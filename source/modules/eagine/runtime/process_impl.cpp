/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module;

#if __has_include(<sys/types.h>) && __has_include(<unistd.h>)
#include <sys/types.h>
#include <unistd.h>
#ifndef EAGINE_POSIX
#define EAGINE_POSIX 1
#endif
#else
#ifndef EAGINE_POSIX
#define EAGINE_POSIX 0
#endif
#endif

module eagine.core.runtime;
import std;
import eagine.core.types;
import eagine.core.utility;

namespace eagine {
//------------------------------------------------------------------------------
auto make_process_instance_id() noexcept -> process_instance_id_t {
    return
#if EAGINE_POSIX
      integer_hash<process_instance_id_t>(::getpid()) ^
#endif
      integer_hash<process_instance_id_t>(
        std::chrono::steady_clock::now().time_since_epoch().count());
}
//------------------------------------------------------------------------------
auto process_instance_id() noexcept -> process_instance_id_t {
    static const auto pid{make_process_instance_id()};
    return pid;
}
//------------------------------------------------------------------------------
} // namespace eagine
