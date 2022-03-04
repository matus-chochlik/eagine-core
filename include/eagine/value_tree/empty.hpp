/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_VALUE_TREE_EMPTY_HPP
#define EAGINE_VALUE_TREE_EMPTY_HPP

#include "../main_ctx_fwd.hpp"
#include "wrappers.hpp"

namespace eagine::valtree {
//------------------------------------------------------------------------------
/// @brief Creates a compound representing an empty subtree.
/// @ingroup valtree
/// @see from_filesystem_path
/// @see from_json_text
/// @see from_yaml_text
auto empty(main_ctx_parent) -> compound;
//------------------------------------------------------------------------------
} // namespace eagine::valtree

#if !EAGINE_CORE_LIBRARY || defined(EAGINE_IMPLEMENTING_CORE_LIBRARY)
#include <eagine/value_tree/empty.inl>
#endif

#endif // EAGINE_VALUE_TREE_EMPTY_HPP
