/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

export module eagine.core.debug;

import std;
import eagine.core.types;

namespace eagine {
//------------------------------------------------------------------------------
// type name
//------------------------------------------------------------------------------
auto demangle_type_name(const char*) noexcept -> std::string;

/// @brief Returns the demangled name for type T.
/// @ingroup type_utils
/// @note The result is implementation dependent.
export template <typename T>
[[nodiscard]] auto type_name(const T&) noexcept -> std::string {
    return demangle_type_name(typeid(T).name());
}

/// @brief Returns the demangled name for type T.
/// @ingroup type_utils
/// @note The result is implementation dependent.
export template <typename T>
[[nodiscard]] auto type_name() noexcept -> std::string {
    return demangle_type_name(typeid(T).name());
}
//------------------------------------------------------------------------------
// valgrind
//------------------------------------------------------------------------------
export [[nodiscard]] auto running_on_valgrind() noexcept -> tribool;
//------------------------------------------------------------------------------
// slow exec
//------------------------------------------------------------------------------
/// @brief Enumeration indicating (estimated) memory access rate.
export enum class memory_access_rate {
    /// @brief Minimal to none memory access.
    minimal,
    /// @brief Low memory access rate.
    low,
    /// @brief Average access rate.
    medium,
    /// @brief Above average to high access rate.
    high,
    /// @brief  Very high access rate.
    very_high
};

export [[nodiscard]] auto temporal_slowdown_factor(
  const memory_access_rate mem_access = memory_access_rate::medium) noexcept
  -> int {
    if(running_on_valgrind()) [[unlikely]] {
        switch(mem_access) {
            case memory_access_rate::minimal:
                return 5;
            case memory_access_rate::low:
                return 7;
            case memory_access_rate::medium:
                return 12;
            case memory_access_rate::high:
                return 25;
            case memory_access_rate::very_high:
                return 45;
        }
    }
    return 1;
}
//------------------------------------------------------------------------------
} // namespace eagine
