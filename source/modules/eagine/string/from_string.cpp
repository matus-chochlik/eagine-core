/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.string:from_string;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.valid_if;
import :c_str;

namespace eagine {
//------------------------------------------------------------------------------
auto _parse_from_string(const string_view src, long long int&) noexcept -> bool;
//------------------------------------------------------------------------------
export template <std::integral T>
[[nodiscard]] auto parse_from_string(
  const string_view src,
  std::type_identity<T>) noexcept -> optionally_valid<T> {
    long long int parsed{};
    if(_parse_from_string(src, parsed)) {
        return convert_if_fits<T>(parsed);
    }
    return {};
}
//------------------------------------------------------------------------------
auto _parse_from_string(const string_view src, long double&) noexcept -> bool;
//------------------------------------------------------------------------------
export template <std::floating_point T>
[[nodiscard]] auto parse_from_string(
  const string_view src,
  std::type_identity<T>) noexcept -> optionally_valid<T> {
    long double parsed{};
    if(_parse_from_string(src, parsed)) {
        return convert_if_fits<T>(parsed);
    }
    return {};
}
//------------------------------------------------------------------------------
export template <identifier_t V>
[[nodiscard]] auto from_string(
  const string_view src,
  const std::type_identity<bool>,
  const selector<V>) noexcept -> optionally_valid<bool> {

    const string_view true_strs[] = {{"true"}, {"True"}, {"1"}, {"t"}, {"T"}};
    if(find_element(view(true_strs), src)) {
        return {true, true};
    }

    const string_view false_strs[] = {
      {"false"}, {"False"}, {"0"}, {"f"}, {"F"}};
    if(find_element(view(false_strs), src)) {
        return {false, true};
    }

    return {};
}

export template <identifier_t V>
[[nodiscard]] auto from_string(
  const string_view src,
  const std::type_identity<tribool>,
  const selector<V> sel) noexcept -> optionally_valid<tribool> {
    if(const auto val{from_string(src, std::type_identity<bool>{}, sel)}) {
        return {*val, true};
    }

    const string_view unknown_strs[] = {
      {"indeterminate"}, {"Indeterminate"}, {"unknown"}};
    if(find_element(view(unknown_strs), src)) {
        return {indeterminate, true};
    }

    return {};
}

export template <identifier_t V>
[[nodiscard]] auto from_string(
  const string_view src,
  const std::type_identity<char>,
  const selector<V>) noexcept -> optionally_valid<char> {
    if(src.has_value()) {
        return {*src, true};
    }
    return {};
}
//------------------------------------------------------------------------------
template <typename T, typename N>
constexpr auto multiply_and_convert_if_fits(const N n, string_view t) noexcept
  -> optionally_valid<T> {
    if(t.empty()) {
        return convert_if_fits<T>(n);
    } else if(t.has_single_value()) {
        if(t.back() == 'k') {
            return convert_if_fits<T>(n * 1000);
        }
        if(t.back() == 'M') {
            return convert_if_fits<T>(n * 1000000);
        }
        if(t.back() == 'G') {
            return convert_if_fits<T>(n * 1000000000);
        }
    } else if(t.size() == 2) {
        if(t.back() == 'i') {
            if(t.front() == 'K') {
                return convert_if_fits<T>(n * 1024);
            }
            if(t.front() == 'M') {
                return convert_if_fits<T>(n * 1024 * 1024);
            }
            if(t.front() == 'G') {
                return convert_if_fits<T>(n * 1024 * 1024 * 1024);
            }
        }
    }
    return {};
}
//------------------------------------------------------------------------------
auto convert_integer_from_string(
  const string_view src,
  const std::type_identity<short> id,
  const int base) noexcept -> optionally_valid<short>;

auto convert_integer_from_string(
  const string_view src,
  const std::type_identity<int> id,
  const int base) noexcept -> optionally_valid<int>;

auto convert_integer_from_string(
  const string_view src,
  const std::type_identity<long> id,
  const int base) noexcept -> optionally_valid<long>;

auto convert_integer_from_string(
  const string_view src,
  const std::type_identity<long long> id,
  const int base) noexcept -> optionally_valid<long long>;

auto convert_integer_from_string(
  const string_view src,
  const std::type_identity<unsigned short> id,
  const int base) noexcept -> optionally_valid<unsigned short>;

auto convert_integer_from_string(
  const string_view src,
  const std::type_identity<unsigned int> id,
  const int base) noexcept -> optionally_valid<unsigned int>;

auto convert_integer_from_string(
  const string_view src,
  const std::type_identity<unsigned long> id,
  const int base) noexcept -> optionally_valid<unsigned long>;

auto convert_integer_from_string(
  const string_view src,
  const std::type_identity<unsigned long long> id,
  const int base) noexcept -> optionally_valid<unsigned long long>;
//------------------------------------------------------------------------------
export template <std::integral T, identifier_t V>
[[nodiscard]] auto from_string(
  const string_view src,
  const std::type_identity<T> id,
  const selector<V>,
  const int base = 10) noexcept -> optionally_valid<T> {
    return convert_integer_from_string(src, id, base);
}
//------------------------------------------------------------------------------
export template <identifier_t V>
[[nodiscard]] auto from_string(
  const string_view src,
  const std::type_identity<byte>,
  const selector<V> sel) noexcept -> optionally_valid<byte> {
    if(starts_with(src, string_view("0x"))) {
        if(const auto opt_val{from_string(
             skip(src, 2), std::type_identity<unsigned>(), sel, 16)}) {
            return {static_cast<byte>(*opt_val), opt_val <= 255U};
        }
    }
    if(starts_with(src, string_view("0"))) {
        if(const auto opt_val{from_string(
             skip(src, 1), std::type_identity<unsigned>(), sel, 8)}) {
            return {static_cast<byte>(*opt_val), opt_val <= 255U};
        }
    }
    if(const auto opt_val{
         from_string(src, std::type_identity<unsigned>(), sel, 10)}) {
        return {static_cast<byte>(*opt_val), opt_val <= 255U};
    }
    return {};
}
//------------------------------------------------------------------------------
auto convert_float_from_string(
  const string_view src,
  const std::type_identity<float> id) noexcept -> optionally_valid<float>;

auto convert_float_from_string(
  const string_view src,
  const std::type_identity<double> id) noexcept -> optionally_valid<double>;

auto convert_float_from_string(
  const string_view src,
  const std::type_identity<long double> id) noexcept
  -> optionally_valid<long double>;
//------------------------------------------------------------------------------
export template <std::floating_point T, identifier_t V>
[[nodiscard]] auto from_string(
  const string_view src,
  const std::type_identity<T> id,
  const selector<V>) noexcept -> optionally_valid<T> {
    return convert_float_from_string(src, id);
}
//------------------------------------------------------------------------------
export template <identifier_t V>
[[nodiscard]] auto from_string(
  const string_view src,
  const std::type_identity<std::string>,
  const selector<V>) noexcept -> always_valid<std::string> {
    return to_string(src);
}
//------------------------------------------------------------------------------
export template <typename Rep, typename Period, identifier_t V>
[[nodiscard]] auto convert_from_string(
  const string_view src,
  const std::type_identity<std::chrono::duration<Rep, Period>>,
  const selector<V> sel,
  const string_view symbol) noexcept
  -> optionally_valid<std::chrono::duration<Rep, Period>> {
    if(memory::ends_with(src, symbol)) {
        if(const auto opt_val{from_string(
             snip(src, symbol.size()), std::type_identity<Rep>(), sel)}) {
            return {std::chrono::duration<Rep, Period>(*opt_val), true};
        }
    }
    return {};
}
//------------------------------------------------------------------------------
export template <typename Rep, typename Period, identifier_t V>
[[nodiscard]] auto from_string(
  const string_view str,
  const std::type_identity<std::chrono::duration<Rep, Period>>,
  const selector<V> sel) noexcept
  -> optionally_valid<std::chrono::duration<Rep, Period>> {
    using dur_t = std::chrono::duration<Rep, Period>;

    if(const auto d{convert_from_string(
         str,
         std::type_identity<std::chrono::duration<Rep, std::ratio<1>>>(),
         sel,
         "s")}) {
        return {std::chrono::duration_cast<dur_t>(*d), true};
    }
    if(const auto d{convert_from_string(
         str,
         std::type_identity<std::chrono::duration<Rep, std::milli>>(),
         sel,
         "ms")}) {
        return {std::chrono::duration_cast<dur_t>(*d), true};
    }
    if(const auto d{convert_from_string(
         str,
         std::type_identity<std::chrono::duration<Rep, std::micro>>(),
         sel,
         "μs")}) {
        return {std::chrono::duration_cast<dur_t>(*d), true};
    }
    if(const auto d{convert_from_string(
         str,
         std::type_identity<std::chrono::duration<Rep, std::nano>>(),
         sel,
         "ns")}) {
        return {std::chrono::duration_cast<dur_t>(*d), true};
    }
    if(const auto d{convert_from_string(
         str,
         std::type_identity<std::chrono::duration<Rep, std::ratio<60>>>(),
         sel,
         "min")}) {
        return {std::chrono::duration_cast<dur_t>(*d), true};
    }
    if(const auto d{convert_from_string(
         str,
         std::type_identity<std::chrono::duration<Rep, std::ratio<3600>>>(),
         sel,
         "hr")}) {
        return {std::chrono::duration_cast<dur_t>(*d), true};
    }
    if(const auto d{convert_from_string(
         str,
         std::type_identity<std::chrono::duration<Rep, std::ratio<86400LL>>>(),
         sel,
         "dy")}) {
        return {std::chrono::duration_cast<dur_t>(*d), true};
    }
    if(const auto d{convert_from_string(
         str,
         std::type_identity<std::chrono::duration<Rep, std::ratio<31556952LL>>>(),
         sel,
         "yr")}) {
        return {std::chrono::duration_cast<dur_t>(*d), true};
    }
    if(const auto d{convert_from_string(
         str,
         std::type_identity<std::chrono::duration<Rep, std::ratio<31556952LL>>>(),
         sel,
         "a")}) {
        return {std::chrono::duration_cast<dur_t>(*d), true};
    }
    return {};
}
//------------------------------------------------------------------------------
export template <typename T, typename Policy, typename Log, identifier_t V>
[[nodiscard]] auto from_string(
  const string_view str,
  const std::type_identity<basic_valid_if<T, Policy, Log>>,
  const selector<V> sel) noexcept -> optionally_valid<T> {
    if(auto temp{from_string(str, std::type_identity<T>(), sel)}) {
        return {std::move(*temp), Policy()(*temp)};
    }
    return {};
}
//------------------------------------------------------------------------------
/// @brief Converts the string representation in @p src to a value of type @p T.
/// @ingroup type_utils
/// @see is_within_limits
export template <typename T>
[[nodiscard]] auto from_string(const string_view src) noexcept {
    return from_string(src, std::type_identity<T>(), default_selector);
}
//------------------------------------------------------------------------------
/// @brief Converts the string representation in @p src to a value of type @p T.
/// @ingroup type_utils
/// @see is_within_limits
export template <typename T, identifier_t V>
[[nodiscard]] auto from_strings(
  const span<const string_view> src,
  const std::type_identity<T> tid,
  const selector<V> sel) noexcept -> decltype(from_string(*src, tid, sel)) {
    if(src.has_value()) {
        return from_string(*src, tid, sel);
    }
    return {};
}

/// @brief Converts the string representation in @p src to a value of type @p T.
/// @ingroup type_utils
/// @see is_within_limits
export template <typename T>
[[nodiscard]] auto from_strings(const span<const string_view> src) noexcept
  -> decltype(from_strings(src, std::type_identity<T>(), default_selector)) {
    return from_strings(src, std::type_identity<T>(), default_selector);
}
//------------------------------------------------------------------------------
export template <typename T, identifier_t V>
[[nodiscard]] auto assign_if_fits(
  const string_view src,
  T& dst,
  const selector<V> sel = default_selector) noexcept -> bool {
    if(auto conv{from_string(src, std::type_identity<T>{}, sel)}) {
        dst = std::move(*conv);
        return true;
    }
    return false;
}

export template <typename T>
[[nodiscard]] auto assign_if_fits(const string_view src, T& dst) noexcept
  -> bool {
    return assign_if_fits(src, dst, default_selector);
}

export template <typename T, identifier_t V>
[[nodiscard]] auto assign_if_fits(
  const string_view src,
  std::optional<T>& dst,
  const selector<V> sel = default_selector) noexcept -> bool {
    if(auto conv{from_string(src, std::type_identity<T>{}, sel)}) {
        dst = std::move(*conv);
        return true;
    }
    return false;
}

export template <typename T>
[[nodiscard]] auto assign_if_fits(
  const string_view src,
  std::optional<T>& dst) noexcept -> bool {
    return assign_if_fits(src, dst, default_selector);
}

export template <typename T, identifier_t V>
[[nodiscard]] auto assign_if_fits(
  const memory::span<const string_view> src,
  T& dst,
  const selector<V> sel = default_selector) noexcept -> bool {
    if(auto conv{from_strings(src, std::type_identity<T>{}, sel)}) {
        dst = std::move(*conv);
        return true;
    }
    return false;
}

export template <typename T>
[[nodiscard]] auto assign_if_fits(
  const memory::span<const string_view> src,
  T& dst) noexcept -> bool {
    return assign_if_fits(src, dst, default_selector);
}

export template <typename T, identifier_t V>
[[nodiscard]] auto assign_if_fits(
  const memory::span<const string_view> src,
  std::optional<T>& dst,
  const selector<V> sel = default_selector) noexcept -> bool {
    if(auto conv{from_strings(src, std::type_identity<T>{}, sel)}) {
        dst = std::move(*conv);
        return true;
    }
    return false;
}

export template <typename T>
[[nodiscard]] auto assign_if_fits(
  const memory::span<const string_view> src,
  std::optional<T>& dst) noexcept -> bool {
    return assign_if_fits(src, dst, default_selector);
}

export template <typename X, typename T, identifier_t V>
[[nodiscard]] constexpr auto assign_if_fits(
  placeholder_expression<X> e,
  T& dst,
  const selector<V> sel) noexcept {
    return placeholder_expression{[e, &dst, sel](auto&&... args) mutable {
        return tribool{
          assign_if_fits(string_view{e(decltype(args)(args)...)}, dst, sel)};
    }};
}

export template <typename X, typename T>
[[nodiscard]] constexpr auto assign_if_fits(
  placeholder_expression<X> e,
  T& dst) noexcept {
    return assign_if_fits(e, dst, default_selector);
}
//------------------------------------------------------------------------------
/// @brief Converts the string representation in @p src to a value of type @p T.
/// @ingroup type_utils
/// @see is_within_limits
///
/// This overload allows to specify a selector that can change the value
/// conversion rules.
export template <typename T, identifier_t V>
[[nodiscard]] auto from_string(
  const string_view src,
  const selector<V> sel) noexcept {
    return from_string(src, std::type_identity<T>(), sel);
}

export template <typename X, typename T, identifier_t V>
[[nodiscard]] constexpr auto from_string(
  placeholder_expression<X> e,
  const std::type_identity<T> tid,
  const selector<V> sel) noexcept {
    return placeholder_expression{[e, tid, sel](auto&&... args) mutable {
        return from_string(string_view{e(decltype(args)(args)...)}, tid, sel);
    }};
}

export template <typename T, typename X, identifier_t V>
[[nodiscard]] constexpr auto from_string(
  placeholder_expression<X> e,
  const selector<V> sel) noexcept {
    return from_string(std::move(e), std::type_identity<T>(), sel);
}

export template <typename T, typename X>
[[nodiscard]] constexpr auto from_string(placeholder_expression<X> e) noexcept {
    return from_string(std::move(e), std::type_identity<T>(), default_selector);
}
//------------------------------------------------------------------------------
/// @brief Checks if the string can be converted to a value equal to the one given.
/// @ingroup type_utils
/// @see from_string
///
/// This overload allows to specify a selector that can change the value
/// conversion rules.
export template <typename T, identifier_t V>
[[nodiscard]] auto string_has_value(
  const string_view str,
  const T& value,
  const selector<V> sel) noexcept -> bool {
    if(const auto converted{from_string(str, std::type_identity<T>(), sel)}) {
        return are_equal(*converted, value);
    }
    return false;
}

export template <typename T, identifier_t V>
[[nodiscard]] constexpr auto string_has_value(
  const string_view str,
  const string_view value,
  const selector<V>) noexcept -> bool {
    return str == value;
}

export template <std::size_t L, identifier_t V>
[[nodiscard]] constexpr auto string_has_value(
  const string_view str,
  const char (&value)[L],
  const selector<V>) noexcept -> bool {
    return str == string_view{value};
}

/// @brief Checks if the string can be converted to a value equal to the one given.
/// @ingroup type_utils
/// @see from_string
export template <typename T>
[[nodiscard]] auto string_has_value(
  const string_view str,
  const T& value) noexcept -> bool {
    return string_has_value(str, value, default_selector);
}

export template <typename X, typename T, identifier_t V>
[[nodiscard]] constexpr auto string_has_value(
  placeholder_expression<X> e,
  const T& value,
  const selector<V> sel) noexcept {
    return placeholder_expression{[e, value, sel](auto&&... args) mutable {
        return tribool{string_has_value(
          string_view{e(decltype(args)(args)...)}, value, sel)};
    }};
}

export template <typename X, typename T>
[[nodiscard]] constexpr auto string_has_value(
  placeholder_expression<X> e,
  const T& value) noexcept {
    return string_has_value(e, value, default_selector);
}
//------------------------------------------------------------------------------
} // namespace eagine
