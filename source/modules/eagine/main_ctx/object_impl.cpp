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
