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
} // namespace eagine
