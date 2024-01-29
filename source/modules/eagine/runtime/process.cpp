/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.runtime:process;
import eagine.core.types;

namespace eagine {
//------------------------------------------------------------------------------
export auto process_instance_id() noexcept -> process_instance_id_t;
//------------------------------------------------------------------------------
} // namespace eagine
