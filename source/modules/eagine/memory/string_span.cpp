/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.memory:string_span;

import std;
import eagine.core.types;
import :span;

namespace eagine {
namespace memory {
//------------------------------------------------------------------------------
// basic_string_span
//------------------------------------------------------------------------------
/// @brief Specialization of memory::basic_span for character string containers.
/// @ingroup string_utils
/// @see string_view
/// @see string_literal
export template <typename C, typename P = C*, typename S = span_size_t>
class basic_string_span : public basic_span<C, P, S> {
    using base = basic_span<C, P, S>;

    static consteval auto _ce_strlen(const char* str) noexcept {
        span_size_t len = 0U;
        while(str and *str) {
            ++len;
            ++str;
        }
        return len;
    }

    static constexpr auto _get_size(auto& str) noexcept -> S {
        if constexpr(requires { str.zero_terminated_size(); }) {
            return limit_cast<S>(str.zero_terminated_size());
        } else {
            return limit_cast<S>(str.size());
        }
    }

public:
    /// @brief Related standard string type.
    using string_type = std::basic_string<std::remove_const_t<C>>;

    /// @brief Related standard string view type.
    using std_view_type = std::basic_string_view<std::remove_const_t<C>>;

    /// @brief Default constructor.
    /// @post empty
    constexpr basic_string_span() noexcept = default;
    constexpr basic_string_span(const basic_string_span&) noexcept = default;
    constexpr auto operator=(const basic_string_span&) noexcept
      -> basic_string_span& = default;
    constexpr ~basic_string_span() noexcept = default;

    constexpr basic_string_span(P addr, const S length) noexcept
      : base{addr, length} {}

    template <typename R>
    consteval basic_string_span(immediate_function_t, R addr) noexcept
        requires(
          not std::is_array_v<R> and std::is_convertible_v<R, P> and
          std::is_same_v<std::remove_const_t<std::remove_pointer_t<R>>, char>)
      : base{addr, -static_cast<S>(_ce_strlen(addr))} {}

    template <typename R>
    constexpr explicit basic_string_span(R addr) noexcept
        requires(
          not std::is_array_v<R> and std::is_convertible_v<R, P> and
          std::is_same_v<std::remove_const_t<std::remove_pointer_t<R>>, char>)
      : base{addr, addr ? -limit_cast<S>(std::strlen(addr)) : 0} {}

    /// @brief Construction from C string literal
    template <std::size_t N>
    constexpr basic_string_span(C (&array)[N]) noexcept
      : base{
          static_cast<P>(array),
          array[N - 1] == C(0) ? 1 - limit_cast<S>(N) : limit_cast<S>(N)} {}

    /// @brief Construction from C-array and explicit length value.
    template <std::size_t N>
    constexpr basic_string_span(C (&array)[N], const span_size_t n) noexcept
      : base{static_cast<P>(array), limit_cast<S>(n)} {}

    /// @brief Construction from related standard string type.
    constexpr basic_string_span(const string_type& str) noexcept
      : base{static_cast<P>(str.c_str()), -limit_cast<S>(str.size())} {}

    /// @brief Construction from compatible container reference.
    ///
    /// The container passed as argument should have @c data and @c size
    /// member functions with the same semantics as std::string does.
    template <span_source<P, S> Str>
        requires(does_not_hide<Str, basic_string_span>)
    constexpr basic_string_span(Str& str) noexcept
      : base{static_cast<P>(str.data()), _get_size(str)} {}

    /// @brief Construction from compatible container const reference.
    ///
    /// The container passed as argument should have @c data and @c size
    /// member functions with the same semantics as std::string does.
    template <const_span_source<P, S> Str>
        requires(does_not_hide<Str, basic_string_span>)
    constexpr basic_string_span(const Str& str) noexcept
      : base{static_cast<P>(str.data()), _get_size(str)} {}

    using base::data;
    using base::empty;
    using base::size;

    friend constexpr auto operator==(
      basic_string_span l,
      basic_string_span r) noexcept -> bool {
        return are_equal(l, r);
    }

    friend constexpr auto operator!=(
      basic_string_span l,
      basic_string_span r) noexcept -> bool {
        return not are_equal(l, r);
    }

    /// @brief Named conversion to the corresponding standard string view.
    constexpr auto std_view() const noexcept -> std_view_type {
        return {this->data(), this->std_size()};
    }

    /// @brief Implicit conversion to the corresponding standard string view.
    constexpr operator std_view_type() const noexcept {
        return std_view();
    }

    /// @brief Conversion to the corresponding standard string.
    constexpr auto to_string() const -> string_type {
        return {this->data(), this->std_size()};
    }
};
//------------------------------------------------------------------------------
/// @brief Alias for mutable string spans.
/// @ingroup string_utils
export using string_span = basic_string_span<char>;

/// @brief Alias for const string views.
/// @ingroup string_utils
export using string_view = basic_string_span<const char>;
//------------------------------------------------------------------------------
/// @brief Converts a basic_span of characters to string_span.
/// @ingroup string_utils
export template <typename C, typename P, typename S>
constexpr auto std_view(const basic_span<C, P, S> spn) noexcept
  -> std::basic_string_view<std::remove_const_t<C>> {
    return {spn.data(), spn.std_size()};
}
//------------------------------------------------------------------------------
/// @brief Converts a basic_span of characters to standard string.
/// @ingroup string_utils
export template <typename C, typename P, typename S>
constexpr auto to_string(const basic_span<C, P, S> spn)
  -> std::basic_string<std::remove_const_t<C>> {
    return {spn.data(), spn.std_size()};
}
//------------------------------------------------------------------------------
/// @brief Assigns the contents of a span of characters to a standard string.
/// @ingroup string_utils
export template <typename C, typename T, typename A, typename P, typename S>
constexpr auto assign_to(
  const basic_span<const C, P, S> spn,
  std::basic_string<C, T, A>& str) -> auto& {
    str.assign(spn.data(), spn.std_size());
    return str;
}
//------------------------------------------------------------------------------
/// @brief Appends the contents of a span of characters to a standard string.
/// @ingroup string_utils
export template <typename C, typename T, typename A, typename P, typename S>
constexpr auto append_to(
  const basic_span<const C, P, S> spn,
  std::basic_string<C, T, A>& str) -> auto& {
    str.append(spn.data(), spn.std_size());
    return str;
}

export template <typename C, typename T, typename A>
constexpr auto append_to(
  const std::basic_string<C, T, A> src,
  std::basic_string<C, T, A>& dst) -> auto& {
    dst.append(src.data(), src.size());
    return dst;
}
//------------------------------------------------------------------------------
export template <typename C, typename PF, typename PT, typename SF, typename ST>
/// @brief Copies as much from source span to destination, ensures zero termination.
/// @ingroup string_utils
constexpr auto copy_what_fits(
  const basic_string_span<const C, PF, SF> from,
  basic_span<C, PT, ST> to) noexcept -> basic_span<C, PT, ST> {
    if(not to.empty()) [[likely]] {
        const auto size{std::min(from.size(), to.size() - 1)};
        auto end = from.begin();
        std::advance(end, size);
        auto dst = to.begin();
        std::copy(from.begin(), end, dst);
        std::advance(dst, size);
        *dst = '\0';
        return head(to, size);
    }
    return to;
}
//------------------------------------------------------------------------------
/// @brief Structural type wrapper for string literals.
/// @ingroup string_utils
/// @see string_view
export template <std::size_t N>
struct string_literal : std::array<const char, N> {

    template <std::size_t... I>
    constexpr string_literal(
      const char (&s)[N],
      std::index_sequence<I...>) noexcept
      : std::array<const char, N>{{s[I]...}} {}

    /// @brief Construction from a string literal (or char array).
    constexpr string_literal(const char (&s)[N]) noexcept
      : string_literal{s, std::make_index_sequence<N>()} {}

    /// @brief Conversion to string view.
    constexpr operator string_view() const noexcept {
        return string_view{this->data(), span_size(this->size()) - 1};
    }
};
//------------------------------------------------------------------------------
} // namespace memory
//------------------------------------------------------------------------------
using memory::string_literal;
//------------------------------------------------------------------------------
// are_equal helper
//------------------------------------------------------------------------------
export template <typename T, typename P, typename S>
struct cmp_decay_to<memory::basic_string_span<T, P, S>>
  : std::type_identity<memory::basic_span<T, P, S>> {};
//------------------------------------------------------------------------------
// less
//------------------------------------------------------------------------------
/// @brief Comparator template for string span - string span comparisons.
/// @ingroup string_utils
export template <typename Spn>
struct basic_view_less {
    using is_transparent = std::true_type;

    constexpr auto _helper(
      const int cmp,
      const span_size_t lsz,
      const span_size_t rsz) const noexcept -> bool {
        return cmp < 0 ? true : cmp > 0 ? false : (lsz < rsz);
    }

    constexpr auto operator()(const Spn l, const Spn r) const noexcept -> bool {
        return _helper(
          std::strncmp(
            l.data(), r.data(), std::min(l.std_size(), r.std_size())),
          l.size(),
          r.size());
    }
};
//------------------------------------------------------------------------------
/// @brief Comparator template for standard string - string span comparisons.
/// @ingroup string_utils
export template <typename Str, typename Spn>
struct basic_str_view_less : basic_view_less<Spn> {
    using base = basic_view_less<Spn>;

    using base::operator();

    constexpr auto operator()(const Str& l, const Str& r) const noexcept
      -> bool {
        return l < r;
    }

    constexpr auto operator()(const Str& l, const Spn r) const noexcept
      -> bool {
        return (*this)(Spn(l), r);
    }

    constexpr auto operator()(const Spn l, const Str& r) const noexcept
      -> bool {
        return (*this)(l, Spn(r));
    }
};
//------------------------------------------------------------------------------
/// @brief Comparator template for standard string - string_view comparisons.
/// @ingroup string_utils
export using str_view_less =
  basic_str_view_less<std::string, memory::string_view>;
//------------------------------------------------------------------------------
export template <typename C, typename T, typename A, typename Transform>
auto make_span_putter(
  span_size_t& i,
  std::basic_string<C, T, A>& str,
  Transform transform) {
    return [&i, &str, transform](auto value) mutable -> bool {
        if(i < span_size_t(str.size())) {
            if(auto transformed{transform(value)}) {
                str[i++] = std::move(*transformed);
                return true;
            }
        }
        return false;
    };
}
//------------------------------------------------------------------------------
export using memory::string_view;
export using memory::append_to;
//------------------------------------------------------------------------------
export template <typename T>
concept convertible_from_string =
  requires(string_traits<T> st, const string_view src) { st.from(src); };
//------------------------------------------------------------------------------
export template <convertible_from_string T>
[[nodiscard]] constexpr auto from_string(
  const string_view src,
  std::type_identity<T> = {}) noexcept {
    return string_traits<T>{}.from(src);
}
//------------------------------------------------------------------------------
} // namespace eagine

