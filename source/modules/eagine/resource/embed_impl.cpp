/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.resource;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.runtime;

namespace eagine {
//------------------------------------------------------------------------------
auto lazy_search_embedded_resource(identifier_t) noexcept -> embedded_resource {
    return {};
}
//------------------------------------------------------------------------------
} // namespace eagine

