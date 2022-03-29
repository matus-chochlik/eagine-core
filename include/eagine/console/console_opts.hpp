/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_LOGGING_CONSOLE_OPTS_HPP
#define EAGINE_LOGGING_CONSOLE_OPTS_HPP

#include "backend.hpp"
#include <memory>

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Structure holding console customization options.
/// @ingroup console
struct console_options {
    /// @brief The explicitly-specified backend to be used by the console.
    std::unique_ptr<console_backend> forced_backend{};
};
//------------------------------------------------------------------------------
} // namespace eagine

#endif // EAGINE_LOGGING_CONSOLE_OPTS_HPP
