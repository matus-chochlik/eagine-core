/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#ifndef EAGINE_PROCESS_HPP
#define EAGINE_PROCESS_HPP

#include "config/platform.hpp"
#include "identifier_t.hpp"
#include "integer_hash.hpp"
#include <chrono>
#include <cstdint>

#if EAGINE_POSIX
#include <sys/types.h>
#include <unistd.h>
#endif

namespace eagine {
//------------------------------------------------------------------------------
static inline auto make_process_instance_id() noexcept
  -> process_instance_id_t {
    return
#if EAGINE_POSIX
      integer_hash<process_instance_id_t>(::getpid()) ^
#endif
      integer_hash<process_instance_id_t>(
        std::chrono::steady_clock::now().time_since_epoch().count());
}
//------------------------------------------------------------------------------
} // namespace eagine

#endif // EAGINE_PROCESS_HPP
