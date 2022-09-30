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

    /// @brief Indicates if the resource is empty.
    constexpr explicit operator bool() const noexcept {
        return !_res_blk.empty();
    }

    constexpr auto embedded_block() const noexcept -> memory::const_block {
        return _res_blk;
    }

    /// @brief Implicit conversion to const block.
    constexpr operator memory::const_block() const noexcept {
        assert(!is_packed());
        return _res_blk;
    }

    /// @brief Unpacks and appends this resource into a buffer using the provided compressor.
    /// @see fetch
    /// @see is_packed
    auto append_to(data_compressor& comp, memory::buffer& buf) const
      -> memory::const_block {
        if(is_packed()) {
            return {comp.decompress(_res_blk, buf)};
        }
        return memory::append_to(_res_blk, buf);
    }

    /// @brief Unpacks this resource into a buffer using the provided compressor.
    /// @see fetch
    /// @see is_packed
    auto unpack(data_compressor& comp, memory::buffer& buf) const
      -> memory::const_block {
        return append_to(comp, buf.clear());
    }

    /// @brief Unpacks this resource into a buffer using compressor from main context.
    /// @see fetch
    /// @see is_packed
    auto unpack(main_ctx& ctx) const -> memory::const_block {
        return unpack(ctx.compressor(), ctx.scratch_space());
    }

    /// @brief Unpacks this resource using compressor from a main context object.
    /// @see fetch
    /// @see is_packed
    auto unpack(main_ctx_object& mco) const -> memory::const_block {
        return unpack(mco.main_context());
    }

    /// @brief Unpacks this resource and passes the data into the handler function.
    /// @see unpack
    /// @see is_packed
    auto fetch(data_compressor& comp, data_compressor::data_handler handler)
      const -> bool {
        if(is_packed()) {
            return comp.decompress(_res_blk, handler);
        }
        return handler(_res_blk);
    }

    /// @brief Unpacks this resource and passes the data into the handler function.
    /// @see unpack
    /// @see is_packed
    auto fetch(main_ctx& ctx, data_compressor::data_handler handler) {
        return fetch(ctx.compressor(), handler);
    }

    /// @brief Unpacks this resource and passes the data into the handler function.
    /// @see unpack
    /// @see is_packed
    auto fetch(main_ctx_object& mco, data_compressor::data_handler handler) {
        return fetch(mco.main_context(), handler);
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

