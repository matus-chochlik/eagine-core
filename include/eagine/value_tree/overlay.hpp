/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_VALUE_TREE_OVERLAY_HPP
#define EAGINE_VALUE_TREE_OVERLAY_HPP

#include "../main_ctx_fwd.hpp"
#include "wrappers.hpp"

namespace eagine::valtree {
//------------------------------------------------------------------------------
/// @brief Creates a overlay compound, combining multiple other compounds.
/// @ingroup valtree
auto make_overlay(main_ctx_parent, std::vector<compound_attribute> = {})
  -> compound;
//------------------------------------------------------------------------------
auto add_overlay(compound&, compound_attribute) -> bool;

static inline auto add_overlay(compound& c, const compound& a) -> bool {
    return add_overlay(c, a.root());
}
//------------------------------------------------------------------------------
} // namespace eagine::valtree

#if !EAGINE_CORE_LIBRARY || defined(EAGINE_IMPLEMENTING_CORE_LIBRARY)
#include <eagine/value_tree/overlay.inl>
#endif

#endif // EAGINE_VALUE_TREE_OVERLAY_HPP
