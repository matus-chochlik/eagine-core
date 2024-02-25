/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.container;

import std;
import eagine.core.types;
export import :static_vector;
export import :small_vector;
export import :chunk_list;
export import :object_pool;
export import :flat_set;
export import :flat_map;
export import :trie;
export import :iterator;
export import :wrapping;

namespace eagine {
//------------------------------------------------------------------------------
export template <typename W, typename T, typename A>
constexpr auto find(std::vector<T, A>& v, W&& what) noexcept {
    using std::find;
    return optional_iterator{
      v, find(v.begin(), v.end(), std::forward<W>(what))};
}
//------------------------------------------------------------------------------
} // namespace eagine
