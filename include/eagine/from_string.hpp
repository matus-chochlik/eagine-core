/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_FROM_STRING_HPP
#define EAGINE_FROM_STRING_HPP

#include "c_str.hpp"
#include "is_within_limits.hpp"
#include "memory/span_algo.hpp"
#include "selector.hpp"
#include "tribool.hpp"
#include "valid_if/always.hpp"
#include "valid_if/decl.hpp"
#include <chrono>
#include <concepts>
#include <cstdlib>
#include <type_traits>

namespace eagine {
//------------------------------------------------------------------------------
auto _parse_from_string(const string_view src, long long int&) noexcept -> bool;
//------------------------------------------------------------------------------
template <std::integral T>
static inline auto parse_from_string(
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
template <std::floating_point T>
static inline auto parse_from_string(
  const string_view src,
  std::type_identity<T>) noexcept -> optionally_valid<T> {
    long double parsed{};
    if(_parse_from_string(src, parsed)) {
        return convert_if_fits<T>(parsed);
    }
    return {};
}
//------------------------------------------------------------------------------
template <identifier_t V>
static inline auto from_string(
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

template <identifier_t V>
static inline auto from_string(
  const string_view src,
  const std::type_identity<tribool>,
  const selector<V> sel) noexcept -> optionally_valid<tribool> {
    if(const auto val{from_string(src, std::type_identity<bool>{}, sel)}) {
        return {tribool{extract(val), false}, true};
    }

    const string_view unknown_strs[] = {
      {"indeterminate"}, {"Indeterminate"}, {"unknown"}};
    if(find_element(view(unknown_strs), src)) {
        return {indeterminate, true};
    }

    return {};
}

template <identifier_t V>
static inline auto from_string(
  const string_view src,
  const std::type_identity<char>,
  const selector<V>) noexcept -> optionally_valid<char> {
    if(src.size() == 1) {
        return {extract(src), true};
    }
    return {};
}
//------------------------------------------------------------------------------
template <typename T, typename N>
constexpr auto multiply_and_convert_if_fits(const N n, string_view t) noexcept
  -> optionally_valid<T> {
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
  const std::type_identity<T> tid) noexcept -> optionally_valid<T> {
    char* end = nullptr; // NOLINT(hicpp-vararg)
    const auto cstr{c_str(src)};
    errno = 0;
    const N result{converter(cstr, &end)};
    if((errno != ERANGE) && (end != cstr) && (end != nullptr)) {
        if(auto converted{multiply_and_convert_if_fits<T>(
             result, skip_to(cstr.view(), end))}) {
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
  const std::type_identity<T> tid) noexcept -> optionally_valid<T> {
    char* end = nullptr; // NOLINT(hicpp-vararg)
    const auto cstr{c_str(src)};
    errno = 0;
    const N result = converter(cstr, &end, base);
    if((errno != ERANGE) && (end != cstr) && (end != nullptr)) {
        if(auto converted{multiply_and_convert_if_fits<T>(
             result, skip_to(cstr.view(), end))}) {
            return converted;
        }
    }
    return parse_from_string(src, tid);
}
//------------------------------------------------------------------------------
template <identifier_t V>
static inline auto from_string(
  const string_view src,
  const std::type_identity<short> id,
  const selector<V>,
  const int base = 10) noexcept -> optionally_valid<short> {
    return convert_from_string_with(&std::strtol, base, src, id);
}

template <identifier_t V>
static inline auto from_string(
  const string_view src,
  const std::type_identity<int> id,
  const selector<V>,
  const int base = 10) noexcept -> optionally_valid<int> {
    return convert_from_string_with(&std::strtol, base, src, id);
}

template <identifier_t V>
static inline auto from_string(
  const string_view src,
  const std::type_identity<long> id,
  const selector<V>,
  const int base = 10) noexcept -> optionally_valid<long> {
    return convert_from_string_with(&std::strtol, base, src, id);
}

template <identifier_t V>
static inline auto from_string(
  const string_view src,
  const std::type_identity<long long> id,
  const selector<V>,
  const int base = 10) noexcept -> optionally_valid<long long> {
    return convert_from_string_with(&std::strtoll, base, src, id);
}

template <identifier_t V>
static inline auto from_string(
  const string_view src,
  const std::type_identity<unsigned short> id,
  const selector<V>,
  const int base = 10) noexcept -> optionally_valid<unsigned short> {
    return convert_from_string_with(&std::strtoul, base, src, id);
}

template <identifier_t V>
static inline auto from_string(
  const string_view src,
  const std::type_identity<unsigned int> id,
  const selector<V>,
  const int base = 10) noexcept -> optionally_valid<unsigned int> {
    return convert_from_string_with(&std::strtoul, base, src, id);
}

template <identifier_t V>
static inline auto from_string(
  const string_view src,
  const std::type_identity<unsigned long> id,
  const selector<V>,
  const int base = 10) noexcept -> optionally_valid<unsigned long> {
    return convert_from_string_with(&std::strtoul, base, src, id);
}

template <identifier_t V>
static inline auto from_string(
  const string_view src,
  const std::type_identity<unsigned long long> id,
  const selector<V>,
  const int base = 10) noexcept -> optionally_valid<unsigned long long> {
    return convert_from_string_with(&std::strtoull, base, src, id);
}
//------------------------------------------------------------------------------
template <identifier_t V>
static inline auto from_string(
  const string_view src,
  const std::type_identity<byte>,
  const selector<V> sel) noexcept -> optionally_valid<byte> {
    if(starts_with(src, string_view("0x"))) {
        if(const auto opt_val{from_string(
             skip(src, 2), std::type_identity<unsigned>(), sel, 16)}) {
            return {static_cast<byte>(extract(opt_val)), bool(opt_val <= 255U)};
        }
    }
    if(starts_with(src, string_view("0"))) {
        if(const auto opt_val{from_string(
             skip(src, 1), std::type_identity<unsigned>(), sel, 8)}) {
            return {static_cast<byte>(extract(opt_val)), bool(opt_val <= 255U)};
        }
    }
    if(const auto opt_val{
         from_string(src, std::type_identity<unsigned>(), sel, 10)}) {
        return {static_cast<byte>(extract(opt_val)), bool(opt_val <= 255U)};
    }
    return {};
}
//------------------------------------------------------------------------------
template <identifier_t V>
static inline auto from_string(
  const string_view src,
  const std::type_identity<float> id,
  const selector<V>) noexcept -> optionally_valid<float> {
    return convert_from_string_with(&std::strtof, src, id);
}

template <identifier_t V>
static inline auto from_string(
  const string_view src,
  const std::type_identity<double> id,
  const selector<V>) noexcept -> optionally_valid<double> {
    return convert_from_string_with(&std::strtod, src, id);
}

template <identifier_t V>
static inline auto from_string(
  const string_view src,
  const std::type_identity<long double> id,
  const selector<V>) noexcept -> optionally_valid<long double> {
    return convert_from_string_with(&std::strtold, src, id);
}

template <identifier_t V>
static inline auto from_string(
  const string_view src,
  const std::type_identity<std::string>,
  const selector<V>) noexcept -> always_valid<std::string> {
    return to_string(src);
}
//------------------------------------------------------------------------------
template <typename Rep, typename Period, identifier_t V>
static inline auto convert_from_string(
  const string_view src,
  const std::type_identity<std::chrono::duration<Rep, Period>>,
  const selector<V> sel,
  const string_view symbol) noexcept
  -> optionally_valid<std::chrono::duration<Rep, Period>> {
    if(memory::ends_with(src, symbol)) {
        if(
          const auto opt_val = from_string(
            snip(src, symbol.size()), std::type_identity<Rep>(), sel)) {
            return {std::chrono::duration<Rep, Period>(extract(opt_val)), true};
        }
    }
    return {};
}
//------------------------------------------------------------------------------
template <typename Rep, typename Period, identifier_t V>
static inline auto from_string(
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
        return {std::chrono::duration_cast<dur_t>(extract(d)), true};
    }
    if(const auto d{convert_from_string(
         str,
         std::type_identity<std::chrono::duration<Rep, std::milli>>(),
         sel,
         "ms")}) {
        return {std::chrono::duration_cast<dur_t>(extract(d)), true};
    }
    if(const auto d{convert_from_string(
         str,
         std::type_identity<std::chrono::duration<Rep, std::micro>>(),
         sel,
         "μs")}) {
        return {std::chrono::duration_cast<dur_t>(extract(d)), true};
    }
    if(const auto d{convert_from_string(
         str,
         std::type_identity<std::chrono::duration<Rep, std::nano>>(),
         sel,
         "ns")}) {
        return {std::chrono::duration_cast<dur_t>(extract(d)), true};
    }
    if(const auto d{convert_from_string(
         str,
         std::type_identity<std::chrono::duration<Rep, std::ratio<60>>>(),
         sel,
         "min")}) {
        return {std::chrono::duration_cast<dur_t>(extract(d)), true};
    }
    if(const auto d{convert_from_string(
         str,
         std::type_identity<std::chrono::duration<Rep, std::ratio<3600>>>(),
         sel,
         "hr")}) {
        return {std::chrono::duration_cast<dur_t>(extract(d)), true};
    }
    if(const auto d{convert_from_string(
         str,
         std::type_identity<std::chrono::duration<Rep, std::ratio<86400LL>>>(),
         sel,
         "dy")}) {
        return {std::chrono::duration_cast<dur_t>(extract(d)), true};
    }
    if(const auto d{convert_from_string(
         str,
         std::type_identity<std::chrono::duration<Rep, std::ratio<31556952LL>>>(),
         sel,
         "yr")}) {
        return {std::chrono::duration_cast<dur_t>(extract(d)), true};
    }
    if(const auto d{convert_from_string(
         str,
         std::type_identity<std::chrono::duration<Rep, std::ratio<31556952LL>>>(),
         sel,
         "a")}) {
        return {std::chrono::duration_cast<dur_t>(extract(d)), true};
    }
    return {};
}
//------------------------------------------------------------------------------
/// @brief Converts the string representation in @p src to a value of type @p T.
/// @ingroup type_utils
/// @see is_within_limits
template <typename T>
auto from_string(const string_view src) noexcept {
    return from_string(src, std::type_identity<T>(), default_selector);
}
//------------------------------------------------------------------------------
/// @brief Converts the string representation in @p src to a value of type @p T.
/// @ingroup type_utils
/// @see is_within_limits
///
/// This overload allows to specify a selector that can change the value
/// conversion rules.
template <typename T, identifier_t V>
auto from_string(const string_view src, const selector<V> sel) noexcept {
    return from_string(src, std::type_identity<T>(), sel);
}
//------------------------------------------------------------------------------
} // namespace eagine

#if !EAGINE_CORE_LIBRARY || defined(EAGINE_IMPLEMENTING_CORE_LIBRARY)
#include <eagine/from_string.inl>
#endif

#endif // EAGINE_FROM_STRING_HPP
