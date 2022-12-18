/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.string:from_string;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.valid_if;
import :c_str;
import <chrono>;
import <concepts>;
import <cerrno>;
import <cstdlib>;
import <optional>;
import <ratio>;
import <string>;
import <type_traits>;

namespace eagine {
//------------------------------------------------------------------------------
auto _parse_from_string(const string_view src, long long int&) noexcept -> bool;
//------------------------------------------------------------------------------
export template <std::integral T>
[[nodiscard]] auto parse_from_string(
  const string_view src,
  std::type_identity<T>) noexcept -> std::optional<T> {
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
  std::type_identity<T>) noexcept -> std::optional<T> {
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
  const selector<V>) noexcept -> std::optional<bool> {

    const string_view true_strs[] = {{"true"}, {"True"}, {"1"}, {"t"}, {"T"}};
    if(find_element(view(true_strs), src)) {
        return {true};
    }

    const string_view false_strs[] = {
      {"false"}, {"False"}, {"0"}, {"f"}, {"F"}};
    if(find_element(view(false_strs), src)) {
        return {false};
    }

    return {};
}

export template <identifier_t V>
[[nodiscard]] auto from_string(
  const string_view src,
  const std::type_identity<tribool>,
  const selector<V> sel) noexcept -> std::optional<tribool> {
    if(const auto val{from_string(src, std::type_identity<bool>{}, sel)}) {
        return {tribool{extract(val), false}};
    }

    const string_view unknown_strs[] = {
      {"indeterminate"}, {"Indeterminate"}, {"unknown"}};
    if(find_element(view(unknown_strs), src)) {
        return {indeterminate};
    }

    return {};
}

export template <identifier_t V>
[[nodiscard]] auto from_string(
  const string_view src,
  const std::type_identity<char>,
  const selector<V>) noexcept -> std::optional<char> {
    if(src.size() == 1) {
        return {extract(src)};
    }
    return {};
}
//------------------------------------------------------------------------------
template <typename T, typename N>
constexpr auto multiply_and_convert_if_fits(const N n, string_view t) noexcept
  -> std::optional<T> {
    if(t.empty()) {
        return convert_if_fits<T>(n);
    } else if(t.size() == 1) {
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
template <typename T, typename N>
auto convert_from_string_with(
  N (*converter)(const char*, char**),
  const string_view src,
  const std::type_identity<T> tid) noexcept -> std::optional<T> {
    char* end = nullptr; // NOLINT(hicpp-vararg)
    const auto cstr{c_str(src)};
    errno = 0;
    const N result{converter(cstr, &end)};
    if((errno != ERANGE) && (end != cstr) && (end != nullptr)) {
        if(auto converted{multiply_and_convert_if_fits<T>(
             result, skip_to(cstr.view(), cstr.position_of(end)))}) {
            return converted;
        }
    }

    return parse_from_string(src, tid);
}
//------------------------------------------------------------------------------
template <typename T, typename N>
auto convert_from_string_with(
  N (*converter)(const char*, char**, int),
  const int base,
  const string_view src,
  const std::type_identity<T> tid) noexcept -> std::optional<T> {
    char* end = nullptr; // NOLINT(hicpp-vararg)
    const auto cstr{c_str(src)};
    errno = 0;
    const N result = converter(cstr, &end, base);
    if((errno != ERANGE) && (end != cstr) && (end != nullptr)) {
        if(auto converted{multiply_and_convert_if_fits<T>(
             result, skip_to(cstr.view(), cstr.position_of(end)))}) {
            return converted;
        }
    }
    return parse_from_string(src, tid);
}
//------------------------------------------------------------------------------
export template <identifier_t V>
[[nodiscard]] auto from_string(
  const string_view src,
  const std::type_identity<short> id,
  const selector<V>,
  const int base = 10) noexcept -> std::optional<short> {
    return convert_from_string_with(&std::strtol, base, src, id);
}

export template <identifier_t V>
[[nodiscard]] auto from_string(
  const string_view src,
  const std::type_identity<int> id,
  const selector<V>,
  const int base = 10) noexcept -> std::optional<int> {
    return convert_from_string_with(&std::strtol, base, src, id);
}

export template <identifier_t V>
[[nodiscard]] auto from_string(
  const string_view src,
  const std::type_identity<long> id,
  const selector<V>,
  const int base = 10) noexcept -> std::optional<long> {
    return convert_from_string_with(&std::strtol, base, src, id);
}

export template <identifier_t V>
[[nodiscard]] auto from_string(
  const string_view src,
  const std::type_identity<long long> id,
  const selector<V>,
  const int base = 10) noexcept -> std::optional<long long> {
    return convert_from_string_with(&std::strtoll, base, src, id);
}

export template <identifier_t V>
[[nodiscard]] auto from_string(
  const string_view src,
  const std::type_identity<unsigned short> id,
  const selector<V>,
  const int base = 10) noexcept -> std::optional<unsigned short> {
    return convert_from_string_with(&std::strtoul, base, src, id);
}

export template <identifier_t V>
[[nodiscard]] auto from_string(
  const string_view src,
  const std::type_identity<unsigned int> id,
  const selector<V>,
  const int base = 10) noexcept -> std::optional<unsigned int> {
    return convert_from_string_with(&std::strtoul, base, src, id);
}

export template <identifier_t V>
[[nodiscard]] auto from_string(
  const string_view src,
  const std::type_identity<unsigned long> id,
  const selector<V>,
  const int base = 10) noexcept -> std::optional<unsigned long> {
    return convert_from_string_with(&std::strtoul, base, src, id);
}

export template <identifier_t V>
[[nodiscard]] auto from_string(
  const string_view src,
  const std::type_identity<unsigned long long> id,
  const selector<V>,
  const int base = 10) noexcept -> std::optional<unsigned long long> {
    return convert_from_string_with(&std::strtoull, base, src, id);
}
//------------------------------------------------------------------------------
export template <identifier_t V>
[[nodiscard]] auto from_string(
  const string_view src,
  const std::type_identity<byte>,
  const selector<V> sel) noexcept -> std::optional<byte> {
    if(starts_with(src, string_view("0x"))) {
        if(const auto opt_val{from_string(
             skip(src, 2), std::type_identity<unsigned>(), sel, 16)}) {
            if(opt_val <= 255U) {
                return {static_cast<byte>(extract(opt_val))};
            }
            return {};
        }
    }
    if(starts_with(src, string_view("0"))) {
        if(const auto opt_val{from_string(
             skip(src, 1), std::type_identity<unsigned>(), sel, 8)}) {
            if(opt_val <= 255U) {
                return {static_cast<byte>(extract(opt_val))};
            }
            return {};
        }
    }
    if(const auto opt_val{
         from_string(src, std::type_identity<unsigned>(), sel, 10)}) {
        if(opt_val <= 255U) {
            return {static_cast<byte>(extract(opt_val))};
        }
        return {};
    }
    return {};
}
//------------------------------------------------------------------------------
export template <identifier_t V>
[[nodiscard]] auto from_string(
  const string_view src,
  const std::type_identity<float> id,
  const selector<V>) noexcept -> std::optional<float> {
    return convert_from_string_with(&std::strtof, src, id);
}

export template <identifier_t V>
[[nodiscard]] auto from_string(
  const string_view src,
  const std::type_identity<double> id,
  const selector<V>) noexcept -> std::optional<double> {
    return convert_from_string_with(&std::strtod, src, id);
}

export template <identifier_t V>
[[nodiscard]] auto from_string(
  const string_view src,
  const std::type_identity<long double> id,
  const selector<V>) noexcept -> std::optional<long double> {
    return convert_from_string_with(&std::strtold, src, id);
}

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
  -> std::optional<std::chrono::duration<Rep, Period>> {
    if(memory::ends_with(src, symbol)) {
        if(
          const auto opt_val = from_string(
            snip(src, symbol.size()), std::type_identity<Rep>(), sel)) {
            return {std::chrono::duration<Rep, Period>(extract(opt_val))};
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
  -> std::optional<std::chrono::duration<Rep, Period>> {
    using dur_t = std::chrono::duration<Rep, Period>;

    if(const auto d{convert_from_string(
         str,
         std::type_identity<std::chrono::duration<Rep, std::ratio<1>>>(),
         sel,
         "s")}) {
        return {std::chrono::duration_cast<dur_t>(extract(d))};
    }
    if(const auto d{convert_from_string(
         str,
         std::type_identity<std::chrono::duration<Rep, std::milli>>(),
         sel,
         "ms")}) {
        return {std::chrono::duration_cast<dur_t>(extract(d))};
    }
    if(const auto d{convert_from_string(
         str,
         std::type_identity<std::chrono::duration<Rep, std::micro>>(),
         sel,
         "μs")}) {
        return {std::chrono::duration_cast<dur_t>(extract(d))};
    }
    if(const auto d{convert_from_string(
         str,
         std::type_identity<std::chrono::duration<Rep, std::nano>>(),
         sel,
         "ns")}) {
        return {std::chrono::duration_cast<dur_t>(extract(d))};
    }
    if(const auto d{convert_from_string(
         str,
         std::type_identity<std::chrono::duration<Rep, std::ratio<60>>>(),
         sel,
         "min")}) {
        return {std::chrono::duration_cast<dur_t>(extract(d))};
    }
    if(const auto d{convert_from_string(
         str,
         std::type_identity<std::chrono::duration<Rep, std::ratio<3600>>>(),
         sel,
         "hr")}) {
        return {std::chrono::duration_cast<dur_t>(extract(d))};
    }
    if(const auto d{convert_from_string(
         str,
         std::type_identity<std::chrono::duration<Rep, std::ratio<86400LL>>>(),
         sel,
         "dy")}) {
        return {std::chrono::duration_cast<dur_t>(extract(d))};
    }
    if(const auto d{convert_from_string(
         str,
         std::type_identity<std::chrono::duration<Rep, std::ratio<31556952LL>>>(),
         sel,
         "yr")}) {
        return {std::chrono::duration_cast<dur_t>(extract(d))};
    }
    if(const auto d{convert_from_string(
         str,
         std::type_identity<std::chrono::duration<Rep, std::ratio<31556952LL>>>(),
         sel,
         "a")}) {
        return {std::chrono::duration_cast<dur_t>(extract(d))};
    }
    return {};
}
//------------------------------------------------------------------------------
export template <typename T, typename Policy, typename Log, identifier_t V>
[[nodiscard]] auto from_string(
  const string_view str,
  const std::type_identity<basic_valid_if<T, Policy, Log>>,
  const selector<V> sel) noexcept -> std::optional<T> {
    if(const auto temp{from_string(str, std::type_identity<T>(), sel)}) {
        if(Policy()(*temp)) {
            return temp;
        }
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
export template <typename T>
[[nodiscard]] auto from_strings(const span<const string_view> src) noexcept
  -> decltype(from_string(
    extract(src),
    std::type_identity<T>(),
    default_selector)) {
    if(src.has_single_value()) {
        return from_string(
          extract(src), std::type_identity<T>(), default_selector);
    }
    return {};
}
//------------------------------------------------------------------------------
export template <typename T>
[[nodiscard]] auto assign_if_fits(const string_view src, T& dst) noexcept
  -> bool {
    if(auto conv{from_string<T>(src)}) {
        dst = std::move(extract(conv));
        return true;
    }
    return false;
}

export template <typename T>
[[nodiscard]] auto assign_if_fits(
  const string_view src,
  std::optional<T>& dst) noexcept -> bool {
    if(auto conv{from_string<T>(src)}) {
        dst = std::move(extract(conv));
        return true;
    }
    return false;
}

export template <typename T>
[[nodiscard]] auto assign_if_fits(
  const memory::span<const string_view> src,
  T& dst) noexcept -> bool {
    if(auto conv{from_strings<T>(src)}) {
        dst = std::move(extract(conv));
        return true;
    }
    return false;
}

export template <typename T>
[[nodiscard]] auto assign_if_fits(
  const memory::span<const string_view> src,
  std::optional<T>& dst) noexcept -> bool {
    if(auto conv{from_strings<T>(src)}) {
        dst = std::move(extract(conv));
        return true;
    }
    return false;
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
  const selector<V> sel) noexcept {
    if(const auto converted{from_string(str, std::type_identity<T>(), sel)}) {
        return are_equal(extract(converted), value);
    }
    return false;
}
//------------------------------------------------------------------------------
/// @brief Checks if the string can be converted to a value equal to the one given.
/// @ingroup type_utils
/// @see from_string
export template <typename T>
[[nodiscard]] auto string_has_value(
  const string_view str,
  const T& value) noexcept {
    return string_has_value(str, value, default_selector);
}
//------------------------------------------------------------------------------
} // namespace eagine
