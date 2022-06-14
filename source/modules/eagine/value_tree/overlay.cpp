/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.value_tree:overlay;

import eagine.core.logging;
import :wrappers;
import <vector>;

namespace eagine::valtree {
//------------------------------------------------------------------------------
/// @brief Creates a overlay compound, combining multiple other compounds.
/// @ingroup valtree
export auto make_overlay(logger& parent, std::vector<compound_attribute>)
  -> compound;

export auto make_overlay(logger& parent) -> compound {
    return make_overlay(parent, {});
}
//------------------------------------------------------------------------------
/* TODO: this crashes clang currently
export auto add_overlay(compound&, const compound_attribute&) -> bool;

export auto add_overlay(compound& c, const compound& a) -> bool {
    return add_overlay(c, a.root());
}
*/
//------------------------------------------------------------------------------
} // namespace eagine::valtree

