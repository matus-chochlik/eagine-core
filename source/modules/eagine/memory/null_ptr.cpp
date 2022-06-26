/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.memory:null_ptr;

import eagine.core.types;
import <concepts>;
import <cstdint>;

namespace eagine::memory {
//------------------------------------------------------------------------------
/// @brief Class for doing pointer arithmetic on null pointers.
/// @ingroup type_utils
/// @see typed_nullptr
export template <typename T>
struct typed_nullptr_t {};
//------------------------------------------------------------------------------
/// @brief Template constant for doing pointer arithmetic on null pointers.
/// @ingroup type_utils
export template <typename T>
constexpr typed_nullptr_t<T> typed_nullptr{};
//------------------------------------------------------------------------------
/// @brief Pointer arithmetic addition operator for null pointers.
/// @ingroup type_utils
export template <typename T, std::integral N>
constexpr auto operator+(const typed_nullptr_t<T>, const N n) noexcept -> T* {
    // NOLINTNEXTLINE(performance-no-int-to-ptr)
    return reinterpret_cast<T*>(std::uintptr_t(std_size(n) * sizeof(T)));
}
//------------------------------------------------------------------------------
} // namespace eagine::memory

