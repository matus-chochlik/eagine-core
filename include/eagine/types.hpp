/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_TYPES_HPP
#define EAGINE_TYPES_HPP

#include "is_within_limits.hpp"
#include "type_identity.hpp"
#include "valid_if/nonnegative.hpp"
#include <cassert>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace eagine {

/// @brief Byte type alias.
/// @ingroup type_utils
using byte = unsigned char;

/// @brief Alignment type used by std.
/// @ingroup type_utils
using std_align_t = std::size_t;

/// @brief Size type used by std.
/// @ingroup type_utils
using std_size_t = std::size_t;

/// @brief Signed span size type used by eagine.
/// @ingroup type_utils
using span_size_t = std::ptrdiff_t;

/// @brief Valid size type.
/// @ingroup type_utils
using valid_span_size = valid_if_nonnegative<span_size_t>;

/// @brief Converts argument to std alignment type.
/// @ingroup type_utils
template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
static constexpr auto std_align(const T v) noexcept {
    return limit_cast<std_align_t>(v);
}

/// @brief Converts argument to std size type.
/// @ingroup type_utils
template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
static constexpr auto std_size(const T v) noexcept {
    return limit_cast<std_size_t>(v);
}

/// @brief Converts argument to span size type.
/// @ingroup type_utils
template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
static constexpr auto span_size(const T v) noexcept {
    return limit_cast<span_size_t>(v);
}

/// @brief Returns the byte alignment of type T as span_size_t.
/// @ingroup type_utils
template <typename T>
static constexpr auto span_align_of(const type_identity<T> = {}) noexcept {
    return span_size(alignof(T));
}

/// @brief Returns the byte size of type T as span_size_t.
/// @ingroup type_utils
template <typename T>
static constexpr auto span_size_of(const type_identity<T> = {}) noexcept {
    return span_size(sizeof(T));
}

/// @brief Returns the total byte size of n elements of type T as span_size_t.
/// @ingroup type_utils
template <typename T, typename S>
static constexpr auto span_size_of(
  const S n,
  const type_identity<T> = {}) noexcept {
    return span_size(sizeof(T)) * span_size(n);
}

template <std::integral T>
class integer {
public:
    constexpr integer() noexcept = default;
    constexpr integer(T value) noexcept
      : _value{value} {}

    constexpr operator T() const noexcept {
        return _value;
    }

    template <std::integral I>
    requires(!std::is_same_v<T, I>) constexpr operator I() const noexcept {
        return EAGINE_CONSTEXPR_ASSERT(
          is_within_limits<I>(_value), static_cast<I>(_value));
    }

private:
    T _value{};
};

} // namespace eagine

#endif // EAGINE_TYPES_HPP
