/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.utility:unreachable;

import std;

namespace eagine {
//------------------------------------------------------------------------------
export [[noreturn]] void unreachable() noexcept {
#if defined(__GNUC__)   // GCC, Clang, ICC
    __builtin_unreachable();
#elif defined(_MSC_VER) // MSVC
    __assume(false);
#else
    assert(false);
#endif
}
//------------------------------------------------------------------------------
export template <auto N>
auto unreachable_storage() noexcept -> void* {
    static std::aligned_storage_t<N> _storage{};
    return static_cast<void*>(&_storage);
}
//------------------------------------------------------------------------------
export template <typename T>
auto unreachable_reference(std::type_identity<T>) noexcept -> T& {
    unreachable();
    return *static_cast<T*>(unreachable_storage<sizeof(T)>());
}
//------------------------------------------------------------------------------

} // namespace eagine

