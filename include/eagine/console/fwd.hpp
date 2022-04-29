/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_CONSOLE_FWD_HPP
#define EAGINE_CONSOLE_FWD_HPP

#include <cstdint>

namespace eagine {
//------------------------------------------------------------------------------
class console;

/// @brief Console entry instance id type.
/// @ingroup console
using console_entry_id_t = std::uintptr_t;
//------------------------------------------------------------------------------
} // namespace eagine

#endif // EAGINE_CONSOLE_FWD_HPP
