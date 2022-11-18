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
import eagine.core.reflection;
import eagine.core.utility;
import eagine.core.runtime;
import eagine.core.main_ctx;
import <cstdint>;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Enumeration indicating the base format of embedded resource block.
/// @see embedded resource
/// @see embed
export enum class embedded_resource_format : std::uint8_t {
    /// @brief Unknown binary.
    unknown = 0U,
    /// @brief Conforming XML
    xml,
    /// @brief Conforming JSON.
    json,
    /// @brief JSON header followed by binary data.
    json_binary,
    /// @brief Conforming YAML.
    yaml,
    /// @brief GLSL source.
    glsl,
    /// @brief C++ source.
    cpp,
    /// @brief UTF8 formatted text.
    text_utf8
};

export template <typename Selector>
constexpr auto enumerator_mapping(
  const std::type_identity<embedded_resource_format>,
  const Selector) noexcept {
    return enumerator_map_type<embedded_resource_format, 8>{
      {{"unknown", embedded_resource_format::unknown},
       {"xml", embedded_resource_format::xml},
       {"json", embedded_resource_format::json},
       {"json_binary", embedded_resource_format::json_binary},
       {"yaml", embedded_resource_format::yaml},
       {"glsl", embedded_resource_format::glsl},
       {"cpp", embedded_resource_format::cpp},
       {"text_utf8", embedded_resource_format::text_utf8}}};
}
//------------------------------------------------------------------------------
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
      const embedded_resource_format format,
      const bool packed = false) noexcept
      : _res_blk{blk}
      , _src_path{src_path}
      , _format{format}
      , _packed{packed} {}

    /// @brief Returns the path of the file this resource data comes from.
    constexpr auto source_path() const noexcept -> string_view {
        return _src_path;
    }
    /// @brief Returns the basic data format of the resource.
    /// @see is_packed
    constexpr auto format() const noexcept -> embedded_resource_format {
        return _format;
    }

    /// @brief Indicates if the resource is packed and needs to be decompressed.
    /// @see format
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
    /// @see make_unpacker
    auto unpack(main_ctx_object& mco) const -> memory::const_block {
        return unpack(mco.main_context());
    }

    /// @brief Creates an object that can unpack the resource per-partes.
    /// @see unpack
    auto make_unpacker(
      memory::buffer_pool& buffers,
      block_stream_decompression::data_handler handler,
      span_size_t chunk_size = block_stream_decompression::default_chunk_size())
      const -> block_stream_decompression {
        const auto [method, input]{
          data_compression_method_from_header(embedded_block())};
        return {
          data_compressor{method, buffers}, handler, method, input, chunk_size};
    }

    /// @brief Creates an object that can unpack the resource per-partes.
    /// @see unpack
    auto make_unpacker(
      main_ctx& ctx,
      block_stream_decompression::data_handler handler,
      span_size_t chunk_size = block_stream_decompression::default_chunk_size())
      const -> block_stream_decompression {
        return make_unpacker(ctx.buffers(), handler, chunk_size);
    }

    /// @brief Creates an object that can unpack the resource per-partes.
    /// @see unpack
    auto make_unpacker(
      main_ctx_object& mco,
      block_stream_decompression::data_handler handler,
      span_size_t chunk_size = block_stream_decompression::default_chunk_size())
      const -> block_stream_decompression {
        return make_unpacker(mco.main_context(), handler, chunk_size);
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
    auto fetch(main_ctx& ctx, data_compressor::data_handler handler) const {
        return fetch(ctx.compressor(), handler);
    }

    /// @brief Unpacks this resource and passes the data into the handler function.
    /// @see unpack
    /// @see is_packed
    auto fetch(main_ctx_object& mco, data_compressor::data_handler handler)
      const {
        return fetch(mco.main_context(), handler);
    }

private:
    memory::const_block _res_blk{};
    string_view _src_path{};
    embedded_resource_format _format{embedded_resource_format::unknown};
    bool _packed{false};
};

/// @brief Converts the embedded resource block to a const span of characters.
/// @ingroup embedding
/// @relates embedded_resource
export auto as_chars(const embedded_resource& res) noexcept {
    return as_chars(memory::const_block{res});
}
//------------------------------------------------------------------------------
} // namespace eagine

