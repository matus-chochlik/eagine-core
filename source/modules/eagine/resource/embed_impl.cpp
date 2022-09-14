/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module eagine.core.resource;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.runtime;

namespace eagine {
//------------------------------------------------------------------------------
auto embedded_resource_loader::search(identifier_t res_id) noexcept
  -> embedded_resource {
    if(auto found{_self.find_function(
         "search_embedded_resource",
         std::type_identity<embedded_resource(identifier_t)>())}) {
        if(auto search_func{extract(found)}) {
            return search_func(res_id);
        }
    }
    return {};
}
//------------------------------------------------------------------------------
} // namespace eagine

