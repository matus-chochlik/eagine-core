/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.resource:embedded;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.identifier;
import eagine.core.utility;
import eagine.core.runtime;
import eagine.core.main_ctx;

namespace eagine {

/// @brief Class providing access to a const resource block embedded into the executable.
/// @ingroup embedding
/// @see embed
export class embedded_resource {
public:
    /// @brief Default constructor.
    constexpr embedded_resource() noexcept = default;

    constexpr embedded_resource(
      const memory::const_block blk,
      const string_view src_path,
      const bool packed = false) noexcept
      : _res_blk{blk}
      , _src_path{src_path}
      , _packed{packed} {}

    /// @brief Returns the path of the file this resource data comes from.
    constexpr auto source_path() const noexcept -> string_view {
        return _src_path;
    }

    /// @brief Indicates if the resource is packed and needs to be decompressed.
    /// @see decompress
    constexpr auto is_packed() const noexcept -> bool {
        return _packed;
    }

    /// @brief Implicit conversion to const block.
    constexpr operator memory::const_block() const noexcept {
        assert(!is_packed());
        return _res_blk;
    }

    /// @brief Unpacks this resource into a buffer using the provided compressor.
    /// @see is_packed
    auto unpack(data_compressor& comp, memory::buffer& buf) const
      -> memory::const_block {
        if(is_packed()) {
            return {comp.decompress(_res_blk, buf)};
        }
        return copy_into(_res_blk, buf);
    }

    /// @brief Unpacks this resource into a buffer using compressor from main context.
    /// @see is_packed
    auto unpack(main_ctx& ctx) const -> memory::const_block {
        return unpack(ctx.compressor(), ctx.scratch_space());
    }

    /// @brief Unpacks this resource using compressor from a main context object.
    /// @see is_packed
    auto unpack(main_ctx_object& mco) const -> memory::const_block {
        return unpack(mco.main_context());
    }

private:
    memory::const_block _res_blk{};
    string_view _src_path{};
    bool _packed{false};
};

/// @brief Converts the embedded resource block to a const span of characters.
/// @ingroup embedding
/// @relates embedded_resource
export auto as_chars(const embedded_resource& res) noexcept {
    return as_chars(memory::const_block{res});
}

} // namespace eagine

