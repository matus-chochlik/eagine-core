/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.string:c_str;

import std;
import eagine.core.concepts;
import eagine.core.types;
import eagine.core.memory;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Helper template for getting zero-terminated strings from string spans.
/// @ingroup string_utils
/// @see c_str
///
/// String spans can in many cases internally know if the element past the end
/// is zero or not. There are cases where string spans or generally spans are
/// not zero terminated. Because of this whenever a zero-terminated C-string
/// is used, for example as an argument to a C-API call, instantiations of this
/// template should be used the obtain a valid C-string.
export template <
  typename C,
  typename P = C*,
  typename S = span_size_t,
  bool isConst = true>
class basic_c_str {
public:
    using span_type = memory::basic_string_span<C, P, S>;
    using string_type = std::basic_string<std::remove_const_t<C>>;
    using pointer_type = P;

    constexpr basic_c_str() noexcept = default;

    constexpr basic_c_str(const span_type s) noexcept
      : _span{_get_span(s)}
      , _str{_get_str(s)} {}

    template <extractable E>
    constexpr basic_c_str(construct_from_t, const E& e) noexcept
        requires(has_value_type_v<E, span_type>)
      : _span{_xtr_span(e)}
      , _str{_xtr_str(e)} {}

    /// @brief Returns a zero terminated C-string as pointer_type.
    /// @see view
    [[nodiscard]] constexpr auto c_str() const noexcept -> pointer_type {
        return _span.empty() ? _str.c_str() : _span.data();
    }

    /// @brief Returns the length of the wrapped C-string.
    [[nodiscard]] constexpr auto size() const noexcept -> span_size_t {
        return _span.empty() ? span_size(_str.size()) : _span.size();
    }

    /// @brief Implicit conversion to character pointer_type.
    /// @see c_str
    [[nodiscard]] constexpr operator pointer_type() const noexcept {
        return c_str();
    }

    /// @brief Returns a const view of the string.
    /// @see c_str
    /// @see position_of
    [[nodiscard]] constexpr auto view() const noexcept -> span_type {
        return _span.empty() ? span_type{_str} : _span;
    }

    /// @brief Returns the offset of the given pointer within the C-string.
    /// @see position_of
    [[nodiscard]] constexpr auto offset_of(const char* ptr) const noexcept
      -> std::ptrdiff_t {
        if(ptr < c_str()) {
            return 0;
        } else if(ptr >= c_str() + size()) {
            return size();
        } else [[likely]] {
            return span_size(std::distance(c_str(), ptr));
        }
    }

    /// @brief Returns the position of the given pointer within the C-string.
    /// @see offset_of
    /// @see view
    [[nodiscard]] constexpr auto position_of(const char* ptr) const noexcept
      -> auto {
        return view().begin() + offset_of(ptr);
    }

private:
    static constexpr auto _get_span(const span_type s) noexcept -> span_type {
        return s.is_zero_terminated() ? s : span_type{};
    }

    static constexpr auto _get_str(const span_type s) noexcept -> string_type {
        return s.is_zero_terminated() ? string_type{} : s.to_string();
    }

    template <extractable E>
    static constexpr auto _xtr_span(const E& e) noexcept -> span_type
        requires(
          has_value_type_v<E, span_type> and not std::is_same_v<E, span_type>)
    {
        return has_value(e) ? _get_span(extract(e)) : span_type{};
    }

    template <extractable E>
    static constexpr auto _xtr_str(const E& e) noexcept -> string_type
        requires(has_value_type_v<E, span_type>)
    {
        return has_value(e) ? _get_str(extract(e)) : string_type{};
    }

    std::conditional_t<isConst, const span_type, span_type> _span{};
    std::conditional_t<isConst, const string_type, string_type> _str{};
};

template <typename>
struct get_basic_c_str;

template <typename C, typename P, typename S>
struct get_basic_c_str<memory::basic_string_span<C, P, S>>
  : std::type_identity<basic_c_str<C, P, S>> {};

template <extractable E>
struct get_basic_c_str<E> : get_basic_c_str<extracted_type_t<E>> {};
//------------------------------------------------------------------------------
/// @brief Functions that construct a basic_c_str from a basic_string_span.
/// @ingroup string_utils
export template <typename C, typename P, typename S>
[[nodiscard]] constexpr auto c_str(const memory::basic_span<C, P, S> s) noexcept
  -> basic_c_str<C, P, S>
    requires(std::is_convertible_v<
             memory::basic_span<C, P, S>,
             memory::basic_string_span<C, P, S>>)
{
    return {s};
}

export template <extractable E>
[[nodiscard]] constexpr auto c_str(const E& e) noexcept ->
  typename get_basic_c_str<E>::type {
    return {construct_from, e};
}
//------------------------------------------------------------------------------
} // namespace eagine
