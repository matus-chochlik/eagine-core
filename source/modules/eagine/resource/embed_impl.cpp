/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cstdint>

extern "C" {
struct eagine_embedded_resource_info {
    const char* src_path;
    long src_path_len;
    const unsigned char* data_pointer;
    long data_size;
    bool is_packed;
};

struct eagine_embedded_resources_list {
    const uint64_t* begin;
    const uint64_t* end;
};
} // extern "C"

module eagine.core.resource;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.identifier;
import eagine.core.runtime;

namespace eagine {
//------------------------------------------------------------------------------
auto embedded_resource_loader::search(identifier_value res_id) noexcept
  -> embedded_resource {
    using search_func_type =
      struct eagine_embedded_resource_info(std::uint64_t);
    if(auto found{_self.find_function(
         "eagine_search_embedded_resource",
         std::type_identity<search_func_type>())}) {
        if(auto search_func{extract(found)}) {
            const auto info{search_func(res_id)};
            return {
              memory::const_block{info.data_pointer, info.data_size},
              string_view{info.src_path, info.src_path_len},
              info.is_packed};
        }
    }
    return {};
}
//------------------------------------------------------------------------------
auto embedded_resource_loader::resource_ids() noexcept
  -> span<const identifier_t> {
    using list_func_type = struct eagine_embedded_resources_list();
    if(auto found{_self.find_function(
         "eagine_embedded_resource_ids",
         std::type_identity<list_func_type>())}) {
        if(auto list_func{extract(found)}) {
            const auto list{list_func()};
            return memory::span<const identifier_t>{list.begin, list.end};
        }
    }
    return {};
}
//------------------------------------------------------------------------------
} // namespace eagine

