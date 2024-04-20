/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.runtime:compression;
import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.utility;
import eagine.core.reflection;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Data compression level enumeration.
/// @ingroup main_context
/// @see data_compressor
export enum class data_compression_level : std::uint8_t {
    /// @brief No compression.
    none,
    /// @brief Fastest compression method lowest compression level.
    lowest,
    /// @brief Normal compression level.
    normal,
    /// @brief Slowest compression method highest compression level.
    highest
};
//------------------------------------------------------------------------------
/// @brief Data compression method enumeration.
/// @ingroup main_context
/// @see data_compression_methods
/// @see default_data_compression_method
export enum class data_compression_method : std::uint8_t {
    /// @brief Unknown / invalid compression method.
    unknown = 0U,
    /// @brief No compression.
    none = 1U << 0U,
    /// @brief ZLIB compression.
    zlib = 1U << 1U
};

export template <>
struct enumerator_traits<data_compression_method> {
    static constexpr auto mapping() noexcept {
        return enumerator_map_type<data_compression_method, 3>{
          {{"unknown", data_compression_method::unknown},
           {"none", data_compression_method::none},
           {"zlib", data_compression_method::zlib}}};
    }
};

/// @brief Returns the default data compression method.
/// @ingroup main_context
/// @see data_compression_methods
/// @see default_data_compression_method
export auto default_data_compression_method() noexcept
  -> data_compression_method;

/// @brief Bitfield of compression methods.
/// @ingroup main_context
/// @see data_compression_method
export using data_compression_methods = bitfield<data_compression_method>;

export constexpr auto operator|(
  data_compression_method l,
  data_compression_method r) noexcept -> data_compression_methods {
    return {l, r};
}
//------------------------------------------------------------------------------
export auto data_compression_method_from_header(
  const memory::const_block) noexcept
  -> std::tuple<data_compression_method, memory::const_block>;
//------------------------------------------------------------------------------
struct data_compressor_intf;

/// @brief Class implementing data compression and decompresson.
/// @ingroup main_context
export class data_compressor {
public:
    /// @brief Default constructor.
    data_compressor() noexcept = default;

    /// @brief Initializing constructor.
    data_compressor(memory::buffer_pool&) noexcept;

    /// @brief Construction of a compressor with a specified method.
    data_compressor(data_compression_method, memory::buffer_pool&);

    /// @brief Indicates if this compressor object is initialized and usable.
    explicit operator bool() const noexcept {
        return bool(_impl);
    }

    /// @brief Alias for data handler callable type.
    using data_handler = callable_ref<bool(memory::const_block) noexcept>;

    /// @brief Returns the set of methods supported by the compressor.
    auto supported_methods() const noexcept -> data_compression_methods;

    /// @brief Returns the default compression method of this compressor.
    auto default_method() const noexcept -> data_compression_method;

    /// @brief Examines and skips block header and determines the compression method.
    /// @see supported_method_from_header
    static auto method_from_header(const memory::const_block data) noexcept
      -> std::tuple<data_compression_method, memory::const_block>;

    /// @brief Examines and skips block header and determines the compression method.
    /// @see method_from_header
    auto supported_method_from_header(
      const memory::const_block data) const noexcept
      -> std::tuple<data_compression_method, memory::const_block>;

    /// @brief Begins a compression transaction.
    /// @see compress_next
    /// @see compress_finish
    auto compress_begin(
      const data_compression_method method,
      const data_compression_level level) noexcept -> bool;

    /// @brief Decompresses the next chunk in a compression transaction.
    /// @see compress_begin
    /// @see compress_finish
    auto compress_next(
      const memory::const_block input,
      const data_handler& handler) noexcept -> bool;

    /// @brief Finished the compression transacition.
    /// @see compress_begin
    /// @see compress_next
    auto compress_finish(const data_handler& handler) noexcept -> bool;

    /// @brief Compress the input block, passing the packed data to handler.
    auto compress(
      const data_compression_method method,
      const memory::const_block input,
      const data_handler& handler,
      const data_compression_level level) noexcept -> bool;

    auto compress(
      const memory::const_block input,
      const data_handler& handler,
      const data_compression_level level) noexcept -> bool;

    /// @brief Compress the input block, writing the compressed data to output.
    auto compress(
      const data_compression_method method,
      const memory::const_block input,
      memory::buffer& output,
      const data_compression_level level) noexcept -> memory::const_block;

    auto compress(
      const memory::const_block input,
      memory::buffer& output,
      const data_compression_level level) noexcept -> memory::const_block;

    /// @brief Compress the input block, writing the compressed data to output.
    auto compress(
      const data_compression_method method,
      const memory::const_block input,
      memory::buffer& output) noexcept -> memory::const_block {
        return compress(method, input, output, data_compression_level::normal);
    }

    auto compress(
      const memory::const_block input,
      memory::buffer& output) noexcept -> memory::const_block {
        return compress(input, output, data_compression_level::normal);
    }

    /// @brief Compress the input block into internal buffer, returns const view.
    auto compress(
      const data_compression_method method,
      const memory::const_block input,
      const data_compression_level level) noexcept -> memory::const_block;

    auto default_compress(
      const memory::const_block input,
      const data_compression_level level) noexcept -> memory::const_block;

    /// @brief Begins a decompression transaction.
    /// @see decompress_next
    /// @see decompress_finish
    auto decompress_begin(data_compression_method method) noexcept -> bool;

    /// @brief Decompresses the next chunk in a decompression transaction.
    /// @see decompress_begin
    /// @see decompress_finish
    auto decompress_next(
      const memory::const_block input,
      const data_handler& handler) noexcept -> bool;

    /// @brief Finished the decompression transacition.
    /// @see decompress_begin
    /// @see decompress_next
    auto decompress_finish(const data_handler& handler) noexcept -> bool;

    /// @brief Decompress the input block, passing the unpacked data to handler.
    auto decompress(
      const data_compression_method method,
      const memory::const_block input,
      const data_handler& handler) noexcept -> bool;

    auto decompress(
      const memory::const_block input,
      const data_handler& handler) noexcept -> bool;

    /// @brief Decompress the input block, writing the unpacked data to output.
    auto decompress(
      const data_compression_method method,
      const memory::const_block input,
      memory::buffer& output) noexcept -> memory::const_block;

    auto decompress(
      const memory::const_block input,
      memory::buffer& output) noexcept -> memory::const_block;

    /// @brief Decompress the input block into internal buffer, returns const view.
    auto decompress(
      const data_compression_method method,
      const memory::const_block input) noexcept -> memory::const_block;

    auto default_decompress(const memory::const_block input) noexcept
      -> memory::const_block;

private:
    shared_holder<data_compressor_intf> _impl{};
};
//------------------------------------------------------------------------------
/// @brief Base class for stream compression and decompression helpers
/// @ingroup main_context
export class stream_compression_decompression_base {
public:
    /// @brief Default constructor.
    stream_compression_decompression_base() noexcept = default;

    /// @brief Alias for data handler callable type.
    using data_handler = callable_ref<bool(memory::const_block) noexcept>;

    /// @brief Construction from compressor instance, data handler and compression method.
    stream_compression_decompression_base(
      data_compressor compressor,
      data_handler handler,
      const data_compression_method method) noexcept
      : _compressor{std::move(compressor)}
      , _handler{handler}
      , _method{method} {}

    /// @brief Construction from compressor instance, data handler.
    stream_compression_decompression_base(
      data_compressor compressor,
      data_handler handler) noexcept
      : _compressor{std::move(compressor)}
      , _handler{handler}
      , _method{_compressor.default_method()} {}

    /// @brief Construction from buffer pool, data handler and compression method.
    stream_compression_decompression_base(
      memory::buffer_pool& buffers,
      data_handler handler,
      const data_compression_method method) noexcept
      : stream_compression_decompression_base{
          data_compressor{buffers},
          handler,
          method} {}

    /// @brief Construction from buffer pool, data handler.
    stream_compression_decompression_base(
      memory::buffer_pool& buffers,
      data_handler handler) noexcept
      : stream_compression_decompression_base{
          data_compressor{buffers},
          handler} {}

    /// @brief Indicates if this object is initialized and usable.
    auto is_initialized() const noexcept -> bool {
        return bool{_compressor};
    }

    /// @brief Indicates if compression has started successfully.
    auto has_started() const noexcept -> bool {
        return _started;
    }

    /// @brief Indicates if compression has finished.
    auto has_finished() const noexcept -> bool {
        return _finished;
    }

    /// @brief Indicates if compression has succeeded.
    /// @see has_failed
    auto has_succeeded() const noexcept -> bool {
        return not _failed;
    }

    /// @brief Indicates if compression has failed.
    /// @see has_succeeded
    auto has_failed() const noexcept -> bool {
        return _failed;
    }

    /// @brief Indicates if stream compression is progressing successfully.
    auto is_working() const noexcept -> bool {
        return has_started() and not(has_finished() or has_failed());
    }

    /// @brief Indicates if stream compression is progressing successfully.
    /// @see is_working
    explicit operator bool() const noexcept {
        return is_working();
    }

protected:
    data_compressor _compressor;
    data_handler _handler;
    data_compression_method _method{data_compression_method::unknown};
    bool _started{false};
    bool _finished{false};
    bool _failed{false};
};
//------------------------------------------------------------------------------
/// @brief Class implementing data stream compression functionality.
/// @ingroup main_context
/// @see stream_decompression
export class stream_compression : public stream_compression_decompression_base {
    using base = stream_compression_decompression_base;

public:
    using base::base;

    /// @brief Compresses the next chunk of a data stream to specified level.
    auto next(
      const memory::const_block input,
      data_compression_level level) noexcept -> stream_compression&;

    /// @brief Compresses the next chunk of a data stream to normal level.
    auto next(const memory::const_block input) noexcept -> stream_compression& {
        return next(input, data_compression_level::normal);
    }

    /// @brief Finishes the stream compression.
    auto finish() noexcept -> stream_compression&;
};
//------------------------------------------------------------------------------
/// @brief Class implementing data stream decompression functionality.
/// @ingroup main_context
/// @see stream_compression
/// @see block_stream_decompression
export class stream_decompression
  : public stream_compression_decompression_base {
    using base = stream_compression_decompression_base;

public:
    using base::base;

    /// @brief Decompresses the next chunk of a compressed stream.
    auto next(const memory::const_block input) noexcept
      -> stream_decompression&;

    /// @brief Finishes the stream decompression.
    auto finish() noexcept -> stream_decompression&;
};
//------------------------------------------------------------------------------
/// @brief Class implementing data block decompression functionality.
/// @ingroup main_context
/// @see stream_compression
/// @see stream_decompression
export class block_stream_decompression {
public:
    /// @brief Returns a suitable default chunk size value.
    static constexpr auto default_chunk_size() noexcept -> span_size_t {
        return 64 * 1024;
    }

    /// @brief Alias for data handler callable type.
    using data_handler = stream_decompression::data_handler;

    block_stream_decompression(
      data_compressor compressor,
      data_handler handler,
      data_compression_method method,
      memory::const_block input,
      span_size_t chunk_size = default_chunk_size()) noexcept
      : _base{std::move(compressor), handler, method}
      , _stream{input}
      , _chunk_size{chunk_size} {}

    block_stream_decompression(
      data_compressor compressor,
      data_handler handler,
      std::tuple<data_compression_method, memory::const_block> method_and_input,
      span_size_t chunk_size = default_chunk_size()) noexcept
      : block_stream_decompression{
          std::move(compressor),
          handler,
          std::get<0>(method_and_input),
          std::get<1>(method_and_input),
          chunk_size} {}

    /// @brief Indicates if compression has succeeded.
    auto has_succeeded() const noexcept -> bool {
        return _base.has_succeeded();
    }

    /// @brief Indicates if compression has failed.
    /// @see has_succeeded
    auto has_failed() const noexcept -> bool {
        return _base.has_failed();
    }

    /// @brief Indicates if stream decompression is progressing successfully.
    auto is_working() const noexcept -> bool {
        return _base.is_working();
    }

    /// @brief Indicates if stream decompression is progressing successfully.
    /// @see is_working
    explicit operator bool() const noexcept {
        return is_working();
    }

    /// @brief Decompresses the next chunk of the compressed data.
    auto next() noexcept -> block_stream_decompression& {
        _base.next(head(_stream.tail(), _chunk_size));
        _stream.advance(_chunk_size);
        return *this;
    }

    /// @brief Decompresses next chunk of the data, indicates if should be called again.
    /// @see next
    /// @see is_working
    auto operator()() noexcept -> bool {
        return next().is_working();
    }

private:
    stream_decompression _base;
    memory::const_split_block _stream;
    const span_size_t _chunk_size;
};
//------------------------------------------------------------------------------
} // namespace eagine

