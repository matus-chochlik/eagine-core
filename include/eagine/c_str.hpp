/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_C_STR_HPP
#define EAGINE_C_STR_HPP

#include "extract.hpp"
#include "string_span.hpp"

namespace eagine {
// c_str
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
template <typename C, typename P, typename S>
class basic_c_str {
public:
    using span_type = basic_string_span<C, P, S>;
    using string_type = std::basic_string<std::remove_const_t<C>>;
    using pointer_type = P;

    constexpr basic_c_str(const span_type s) noexcept
      : _span{_get_span(s)}
      , _str{_get_str(s)} {}

    template <extractable E>
    constexpr basic_c_str(const E& e) noexcept
      requires(has_value_type_v<E, span_type>)
      : _span{_xtr_span(e)}
      , _str{_xtr_str(e)} {}

    /// @brief Return a zero terminated C-string as pointer_type.
    /// @see view
    constexpr auto c_str() const noexcept -> pointer_type {
        return _span.empty() ? _str.c_str() : _span.data();
    }

    /// @brief Implicit conversion to character pointer_type.
    /// @see c_str
    constexpr operator pointer_type() const noexcept {
        return c_str();
    }

    /// @brief Returns a const view of the string.
    /// @see c_str()
    constexpr auto view() const noexcept -> span_type {
        return _span.empty() ? span_type{_str} : _span;
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
      requires(has_value_type_v<E, span_type> && !std::is_same_v<E, span_type>) {
        return has_value(e) ? _get_span(extract(e)) : span_type{};
    }

    template <extractable E>
    static constexpr auto _xtr_str(const E& e) noexcept -> string_type
      requires(has_value_type_v<E, span_type>) {
        return has_value(e) ? _get_str(extract(e)) : string_type{};
    }

    const span_type _span{};
    const string_type _str{};
};

template <typename>
struct get_basic_c_str;

template <typename C, typename P, typename S>
struct get_basic_c_str<basic_string_span<C, P, S>> {
    using type = basic_c_str<C, P, S>;
};

template <extractable E>
struct get_basic_c_str<E> : get_basic_c_str<extracted_type_t<E>> {};
//------------------------------------------------------------------------------
/// @brief Functions that construct a basic_c_str from a basic_string_span.
/// @ingroup string_utils
template <typename C, typename P, typename S>
static constexpr auto c_str(const memory::basic_span<C, P, S> s)
  -> basic_c_str<C, P, S> requires(std::is_convertible_v<
                                   memory::basic_span<C, P, S>,
                                   basic_string_span<C, P, S>>) {
    return {s};
}

template <extractable E>
static constexpr auto c_str(const E& e) -> typename get_basic_c_str<E>::type {
    return {e};
}
//------------------------------------------------------------------------------
} // namespace eagine
#endif
