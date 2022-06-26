/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.memory:align;

import eagine.core.types;
import <type_traits>;

namespace eagine::memory {
//------------------------------------------------------------------------------
/// @brief Calculates basic numeric address misalignment.
/// @ingroup memory
export constexpr auto misalignment(
  const span_size_t addr,
  const span_size_t alignment) noexcept -> span_size_t {
    return addr % alignment;
}
//------------------------------------------------------------------------------
/// @brief Calculates basic numeric address misalignment.
/// @ingroup memory
export constexpr auto misalignment(
  const std::nullptr_t,
  const span_size_t) noexcept -> span_size_t {
    return 0;
}
//------------------------------------------------------------------------------
/// @brief Indicates if a basic numeric address is aligned.
/// @ingroup memory
export constexpr auto is_aligned_to(
  const span_size_t addr,
  const span_size_t algn) noexcept {
    return misalignment(addr, algn) == 0;
}
//------------------------------------------------------------------------------
/// @brief Indicates if a basic numeric address is aligned.
/// @ingroup memory
export constexpr auto is_aligned_to(
  const std::nullptr_t,
  const span_size_t) noexcept {
    return true;
}
//------------------------------------------------------------------------------
/// @brief Indicates if a basic numeric address is aligned to alignment of type T.
/// @ingroup memory
export template <typename T>
constexpr auto is_aligned_as(
  const span_size_t addr,
  const std::type_identity<T> = {}) noexcept {
    return is_aligned_to(addr, span_align_of<T>());
}
//------------------------------------------------------------------------------
} // namespace eagine::memory
