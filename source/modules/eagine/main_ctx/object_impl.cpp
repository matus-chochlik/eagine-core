/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module eagine.core.main_ctx;

import std;
import eagine.core.identifier;
import eagine.core.logging;

namespace eagine {
//------------------------------------------------------------------------------
// main_ctx_buffer
//------------------------------------------------------------------------------
auto main_ctx_buffer::get(span_size_t size) noexcept -> main_ctx_buffer& {
    static_cast<memory::buffer&>(*this) =
      _parent.main_context().buffers().get(size);
    return *this;
}
//------------------------------------------------------------------------------
main_ctx_buffer::~main_ctx_buffer() noexcept {
    if(not empty()) {
        _parent.main_context().buffers().eat(
          static_cast<memory::buffer&&>(*this));
    }
}
//------------------------------------------------------------------------------
auto main_ctx_buffer::buffers() const noexcept -> memory::buffer_pool& {
    return _parent.main_context().buffers();
}
//------------------------------------------------------------------------------
// main_ctx_object
//------------------------------------------------------------------------------
auto main_ctx_object::_make_base(
  const identifier obj_id,
  main_ctx_parent parent) noexcept -> base {
    if(parent.has_object()) {
        return base{obj_id, static_cast<const base&>(parent.object())};
    }
    if(parent.has_context()) {
        return base{obj_id, parent.context().log().share_backend()};
    }
    return base{obj_id};
}
//------------------------------------------------------------------------------
} // namespace eagine
