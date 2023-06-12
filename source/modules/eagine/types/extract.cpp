/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.types:extract;

import std;
import :concepts;

namespace eagine {
//------------------------------------------------------------------------------
// pointers
export template <typename T>
struct extract_traits<T*> {
    using value_type = T;
};

/// @brief Checks @p ptr and dereferences it.
/// @pre has_value(ptr)
export template <typename T>
[[nodiscard]] constexpr auto extract(T* ptr) noexcept -> T& {
    assert(ptr);
    return *ptr;
}
//------------------------------------------------------------------------------
} // namespace eagine
