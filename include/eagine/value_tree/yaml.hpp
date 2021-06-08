/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_VALUE_TREE_YAML_HPP
#define EAGINE_VALUE_TREE_YAML_HPP

#include "../config/basic.hpp"
#include "../main_ctx_fwd.hpp"
#include "wrappers.hpp"

namespace eagine::valtree {
//------------------------------------------------------------------------------
/// @brief Creates a compound from a YAML text string view.
/// @ingroup valtree
auto from_yaml_text(string_view, main_ctx_parent) -> compound;
//------------------------------------------------------------------------------
} // namespace eagine::valtree

#if !EAGINE_CORE_LIBRARY || defined(EAGINE_IMPLEMENTING_LIBRARY)
#include <eagine/value_tree/yaml.inl>
#endif

#endif // EAGINE_VALUE_TREE_YAML_HPP
