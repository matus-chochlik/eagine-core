/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.resource:embed;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.identifier;
import :embedded;

namespace eagine {

template <identifier_t ResId>
auto get_embedded_resource(const selector<ResId>, const string_view) noexcept
  -> embedded_resource;

/// @brief Triggers the embedding of data from a file on the specified path.
/// @ingroup embedding
/// @see embedded_resource
///
/// This function only works properly if the build system invokes the embed
/// generator, that generates appropriate C++ code that defines byte blocks
/// with the content of the specified resource files, compiles them and links
/// them into the built executable.
/// The embed generator is installed as one of the tools and in cmake-based
/// build systems the eagine_embed_packed_target_resources and
/// eagine_embed_target_resources cmake functions can be used invoke it.
export template <identifier_value ResId>
auto embed(string_view src_path) noexcept -> embedded_resource {
    return get_embedded_resource(selector<ResId>{}, src_path);
}

} // namespace eagine

