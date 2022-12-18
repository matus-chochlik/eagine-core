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
import eagine.core.runtime;
import :embedded;

namespace eagine {

template <identifier_t ResId>
auto get_embedded_resource(const selector<ResId>, const string_view) noexcept
  -> embedded_resource;

/// @brief Triggers the embedding of data from a file on the specified path.
/// @ingroup embedding
/// @see embedded_resource
/// @see search_resource
///
/// This function only works properly if the build system invokes the embed
/// generator, that generates appropriate C++ code that defines byte blocks
/// with the content of the specified resource files, compiles them and links
/// them into the built executable.
/// The embed generator is installed as one of the tools and in cmake-based
/// build systems the eagine_embed_packed_target_resources and
/// eagine_embed_target_resources cmake functions can be used invoke it.
export template <identifier_value ResId>
[[nodiscard]] auto embed(string_view src_path) noexcept -> embedded_resource {
    return get_embedded_resource(selector<ResId>{}, src_path);
}

[[nodiscard]] auto search_embedded_resource(identifier_t) noexcept
  -> embedded_resource;

/// @brief Class that can be used for searching of embedded resources.
/// @ingroup embedding
/// @see embedded_resource
export class embedded_resource_loader {
public:
    /// @brief Searches embedded resource with the specified id.
    [[nodiscard]] auto search(identifier_value id) noexcept
      -> embedded_resource;

    /// @brief Indicates if a resource with the specified id is embedded.
    [[nodiscard]] auto has_resource(identifier_value id) noexcept -> bool;

    /// @brief Lists the ids of all available embedded resources.
    [[nodiscard]] auto resource_ids() noexcept -> span<const identifier_t>;

    /// @brief Calls a function for each embedded resource.
    /// The functions must take a single embedded_resource argument.
    template <typename Func>
    void for_each(const Func& func) {
        for(const auto res_id : resource_ids()) {
            func(search(res_id));
        }
    }

private:
    shared_executable_module _self{nothing, module_load_option::load_lazy};
};

/// @brief Searches for a resource with the specified identifier.
/// @ingroup embedding
/// @see embedded_resource
/// @see embed
// NOTE: this has to be a template, otherwise there will be link errors.
export template <auto L>
    requires(identifier_literal_length<L>)
[[nodiscard]] auto search_resource(const char (&res_id)[L]) noexcept
  -> embedded_resource {
    return search_embedded_resource(identifier_value(res_id));
}

} // namespace eagine

