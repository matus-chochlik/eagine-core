/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#ifdef __GNUG__
#include <cxxabi.h>
#endif

#if defined(__GNUC__) || defined(__clang__)
// clang-format off
#if __has_include(<valgrind/valgrind.h>)
// clang-format on
#include <valgrind/valgrind.h>
#endif
#endif

module eagine.core.debug;

import std;
import eagine.core.types;

namespace eagine {
//------------------------------------------------------------------------------
// demangle_type_name
//------------------------------------------------------------------------------
[[nodiscard]] auto demangle_type_name(const char* name) noexcept
  -> std::string {
#ifdef __GNUG__
    try {
        int status = -1;

        std::unique_ptr<char, void (*)(void*)> demangled{
          ::abi::__cxa_demangle(name, nullptr, nullptr, &status), ::std::free};

        return (status == 0) ? demangled.get() : name;
    } catch(...) {
    }
#endif
    return name;
}
//------------------------------------------------------------------------------
// valgrind
//------------------------------------------------------------------------------
#if defined(RUNNING_ON_VALGRIND)
[[nodiscard]] auto running_on_valgrind() noexcept -> tribool {
    return bool(RUNNING_ON_VALGRIND); // NOLINT(hicpp-no-assembler)
}
#else
/// @brief Indicates if the current process runs on top of valgrind.
/// @ingroup interop
[[nodiscard]] auto running_on_valgrind() noexcept -> tribool {
    return indeterminate;
}
#endif
//------------------------------------------------------------------------------
} // namespace eagine
