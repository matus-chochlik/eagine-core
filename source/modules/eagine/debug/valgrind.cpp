/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#if defined(__GNUC__) || defined(__clang__)
// clang-format off
#if __has_include(<valgrind/valgrind.h>)
// clang-format on
#include <valgrind/valgrind.h>
#endif
#endif

export module eagine.core.debug:valgrind;

import eagine.core.types;

namespace eagine {

#if defined(RUNNING_ON_VALGRIND)
export [[nodiscard]] auto running_on_valgrind() noexcept -> tribool {
    return bool(RUNNING_ON_VALGRIND); // NOLINT(hicpp-no-assembler)
}
#else
/// @brief Indicates if the current process runs on top of valgrind.
/// @ingroup interop
export [[nodiscard]] constexpr inline auto running_on_valgrind() noexcept
  -> tribool {
    return indeterminate;
}
#endif

} // namespace eagine
