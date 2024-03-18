/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.resource;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.identifier;
import eagine.core.reflection;
import eagine.core.utility;
import eagine.core.runtime;
import eagine.core.logging;
import eagine.core.valid_if;
import eagine.core.value_tree;
import eagine.core.main_ctx;

namespace eagine {
//------------------------------------------------------------------------------
// embedded_resource
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
    /// @brief TTF font.
    ttf,
    /// @brief OTF font.
    otf,
    /// @brief OGG audio clip.
    ogg,
    /// @brief Chaiscript text.
    chai,
    /// @brief UTF8 formatted text.
    text_utf8
};

export template <typename Selector>
constexpr auto enumerator_mapping(
  const std::type_identity<embedded_resource_format>,
  const Selector) noexcept {
    return enumerator_map_type<embedded_resource_format, 12>{
      {{"unknown", embedded_resource_format::unknown},
       {"xml", embedded_resource_format::xml},
       {"json", embedded_resource_format::json},
       {"json_binary", embedded_resource_format::json_binary},
       {"yaml", embedded_resource_format::yaml},
       {"glsl", embedded_resource_format::glsl},
       {"cpp", embedded_resource_format::cpp},
       {"ttf", embedded_resource_format::ttf},
       {"otf", embedded_resource_format::otf},
       {"ogg", embedded_resource_format::ogg},
       {"chai", embedded_resource_format::chai},
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
    [[nodiscard]] constexpr auto source_path() const noexcept -> string_view {
        return _src_path;
    }
    /// @brief Returns the basic data format of the resource.
    /// @see is_packed
    /// @see is_utf8_text
    [[nodiscard]] constexpr auto format() const noexcept
      -> embedded_resource_format {
        return _format;
    }

    /// @brief Indicates if the resource is a UTF-8-encoded text.
    /// @see format
    /// @see is_ttf
    /// @see is_otf
    [[nodiscard]] auto is_utf8_text() const noexcept -> tribool;

    /// @brief Indicates if the resource is a TTF font.
    /// @see format
    /// @see is_otf
    [[nodiscard]] auto is_ttf() const noexcept -> tribool;

    /// @brief Indicates if the resource is a OTF font.
    /// @see format
    /// @see is_ttf
    [[nodiscard]] auto is_otf() const noexcept -> tribool;

    /// @brief Indicates if the resource is a OGG audio clip.
    /// @see format
    [[nodiscard]] auto is_ogg() const noexcept -> tribool;

    /// @brief Indicates if the resource is a Chaiscript file.
    /// @see format
    [[nodiscard]] auto is_chai() const noexcept -> tribool;

    /// @brief Indicates if the resource is packed and needs to be decompressed.
    /// @see format
    /// @see decompress
    [[nodiscard]] constexpr auto is_packed() const noexcept -> bool {
        return _packed;
    }

    /// @brief Indicates if the resource is empty.
    [[nodiscard]] constexpr explicit operator bool() const noexcept {
        return not _res_blk.empty();
    }

    [[nodiscard]] constexpr auto embedded_block() const noexcept
      -> memory::const_block {
        return _res_blk;
    }

    /// @brief Implicit conversion to const block.
    [[nodiscard]] constexpr operator memory::const_block() const noexcept {
        assert(not is_packed());
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
    [[nodiscard]] auto make_unpacker(
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
    [[nodiscard]] auto make_unpacker(
      main_ctx& ctx,
      block_stream_decompression::data_handler handler,
      span_size_t chunk_size = block_stream_decompression::default_chunk_size())
      const -> block_stream_decompression {
        return make_unpacker(ctx.buffers(), handler, chunk_size);
    }

    /// @brief Creates an object that can unpack the resource per-partes.
    /// @see unpack
    [[nodiscard]] auto make_unpacker(
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
    /// @see visit
    auto fetch(main_ctx_object& mco, data_compressor::data_handler handler)
      const {
        return fetch(mco.main_context(), handler);
    }

    /// @brief Visit by the specified visitor if the resource is a value tree.
    /// @see fetch
    /// @see format
    auto visit(
      data_compressor& comp,
      memory::buffer_pool& buffers,
      const logger&,
      shared_holder<valtree::value_tree_visitor> visitor,
      span_size_t max_token_size) const -> bool;

    /// @brief Visit by the specified visitor if the resource is a value tree.
    /// @see fetch
    /// @see build
    /// @see format
    auto visit(
      main_ctx& ctx,
      shared_holder<valtree::value_tree_visitor> visitor,
      span_size_t max_token_size) const -> bool {
        return visit(
          ctx.compressor(),
          ctx.buffers(),
          ctx.log(),
          std::move(visitor),
          max_token_size);
    }

    /// @brief Visit by the specified visitor if the resource is a value tree.
    /// @see fetch
    /// @see build
    /// @see format
    auto visit(
      main_ctx_object& mco,
      shared_holder<valtree::value_tree_visitor> visitor,
      span_size_t max_token_size) const -> bool {
        return visit(mco.main_context(), std::move(visitor), max_token_size);
    }

    /// @brief Apply the specified builder if the resource is a value tree.
    /// @see visit
    /// @see fetch
    /// @see format
    auto build(
      data_compressor& comp,
      memory::buffer_pool& buffers,
      const logger& log,
      shared_holder<valtree::object_builder> builder) const -> bool {
        if(builder) {
            const auto max_token_size{builder->max_token_size()};
            return visit(
              comp,
              buffers,
              log,
              make_building_value_tree_visitor(std::move(builder)),
              max_token_size);
        }
        return false;
    }

    /// @brief Apply the specified builder if the resource is a value tree.
    /// @see visit
    /// @see fetch
    /// @see format
    auto build(main_ctx& ctx, shared_holder<valtree::object_builder> builder)
      const -> bool {
        return build(
          ctx.compressor(), ctx.buffers(), ctx.log(), std::move(builder));
    }

    /// @brief Apply the specified builder if the resource is a value tree.
    /// @see visit
    /// @see fetch
    /// @see format
    auto build(
      main_ctx_object& mco,
      shared_holder<valtree::object_builder> builder) const -> bool {
        return build(mco.main_context(), std::move(builder));
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
// embed
//------------------------------------------------------------------------------
template <identifier_t ResId>
auto get_embedded_resource(const selector<ResId>, const string_view) noexcept
  -> embedded_resource;

export template <identifier_value ResId>
auto embedded(const selector<ResId> sel = {}) noexcept -> embedded_resource {
    return get_embedded_resource(sel, string_view{});
}

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
            func(res_id, search(res_id));
        }
    }

private:
    [[nodiscard]] auto _search_function() noexcept;
    [[nodiscard]] auto _list_function() noexcept;
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
//------------------------------------------------------------------------------
// fetch
//------------------------------------------------------------------------------
export [[nodiscard]] auto fetch_resource(
  const string_view description,
  const string_view key,
  const memory::const_block embedded_blk,
  memory::buffer& buf,
  application_config& cfg,
  const logger& log) -> memory::const_block {

    if(const ok res_path{cfg.get<std::string>(key)}) {
        if(const auto contents{file_contents(res_path.get())}) {
            const auto blk{contents.block()};
            buf.resize(blk.size());
            copy(blk, cover(buf));

            log.debug("using ${resource} from file ${path}")
              .arg("resource", description)
              .arg("key", key)
              .arg("path", "FsPath", res_path);
            log.trace("${resource} content:")
              .arg("resource", description)
              .arg("blob", view(buf));
            return view(buf);
        } else {
            log.error("failed to load ${resource} from file ${path}")
              .arg("resource", description)
              .arg("key", key)
              .arg("path", "FsPath", res_path);
        }
    } else if(embedded_blk) {
        log.debug("using embedded ${resource}").arg("resource", description);
        log.trace("${resource} content:")
          .arg("resource", description)
          .arg("blob", embedded_blk);
        return embedded_blk;
    }

    log.warning("could not fetch ${resource}").arg("resource", description);
    return {};
}
//------------------------------------------------------------------------------
export [[nodiscard]] auto fetch_resource(
  const string_view description,
  const string_view key,
  const memory::const_block embedded_blk,
  main_ctx& ctx) -> memory::const_block {
    return fetch_resource(
      description,
      key,
      embedded_blk,
      ctx.scratch_space(),
      ctx.config(),
      ctx.log());
}
//------------------------------------------------------------------------------
} // namespace eagine
