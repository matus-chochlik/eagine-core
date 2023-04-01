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
    uint8_t format;
    bool is_packed;
};

struct eagine_embedded_resources_list {
    const uint64_t* begin;
    const uint64_t* end;
};
} // extern "C"

module eagine.core.resource;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.identifier;
import eagine.core.runtime;
import eagine.core.logging;
import eagine.core.value_tree;

namespace eagine {
//------------------------------------------------------------------------------
auto embedded_resource::is_utf8_text() const noexcept -> tribool {
    switch(format()) {
        case embedded_resource_format::unknown:
        case embedded_resource_format::xml:
            return indeterminate;
        case embedded_resource_format::json:
        case embedded_resource_format::yaml:
        case embedded_resource_format::glsl:
        case embedded_resource_format::cpp:
        case embedded_resource_format::text_utf8:
            return true;
        default:
            return false;
    }
}
//------------------------------------------------------------------------------
auto embedded_resource::visit(
  data_compressor& comp,
  memory::buffer_pool& buffers,
  const logger& log,
  std::shared_ptr<valtree::value_tree_visitor> visitor,
  span_size_t max_token_size) const -> bool {
    if(
      (format() == embedded_resource_format::json) or
      (format() == embedded_resource_format::json_binary)) {
        auto input{traverse_json_stream(
          std::move(visitor), max_token_size, buffers, log)};
        return fetch(comp, input.get_handler());
    }
    return false;
}
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
              embedded_resource_format{info.format},
              info.is_packed};
        }
    }
    return {};
}
//------------------------------------------------------------------------------
auto embedded_resource_loader::has_resource(identifier_value res_id) noexcept
  -> bool {
    using search_func_type =
      struct eagine_embedded_resource_info(std::uint64_t);
    if(auto found{_self.find_function(
         "eagine_search_embedded_resource",
         std::type_identity<search_func_type>())}) {
        if(auto search_func{extract(found)}) {
            const auto info{search_func(res_id)};
            return info.data_size > 0;
        }
    }
    return false;
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
