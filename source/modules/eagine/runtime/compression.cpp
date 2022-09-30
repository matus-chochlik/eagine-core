/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.runtime:compression;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.utility;
import <cstdint>;
import <memory>;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Data compression level enumeration.
/// @ingroup main_context
/// @see data_compressor
export enum class data_compression_level {
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
struct data_compressor_intf;

/// @brief Class implementing data compression and decompresson.
/// @ingroup main_context
export class data_compressor {
public:
    /// @brief Initializing constructor.
    data_compressor(memory::buffer_pool&) noexcept;

    /// @brief Alias for data handler callable type.
    using data_handler = callable_ref<bool(memory::const_block) noexcept>;

    /// @brief Returns the set of methods supported by the compressor.
    auto supported_methods() const noexcept -> data_compression_methods;

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
    auto compress_finish() noexcept -> bool;

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

    auto compress(
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
    auto decompress_finish() noexcept -> bool;

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

    auto decompress(const memory::const_block input) noexcept
      -> memory::const_block;

private:
    std::shared_ptr<data_compressor_intf> _pimpl{};
};
//------------------------------------------------------------------------------
export class stream_decompression {
public:
    /// @brief Alias for data handler callable type.
    using data_handler = callable_ref<bool(memory::const_block) noexcept>;

    stream_decompression(
      data_compressor compressor,
      data_handler handler,
      const data_compression_method method) noexcept
      : _compressor{std::move(compressor)}
      , _handler{handler}
      , _method{method} {}

    stream_decompression(
      memory::buffer_pool& buffers,
      data_handler handler,
      const data_compression_method method) noexcept
      : stream_decompression{data_compressor{buffers}, handler, method} {}

    stream_decompression(
      memory::buffer_pool& buffers,
      data_handler handler) noexcept
      : stream_decompression{
          {buffers},
          handler,
          default_data_compression_method()} {}

    auto has_started() const noexcept -> bool {
        return _started;
    }

    auto has_finished() const noexcept -> bool {
        return _finished;
    }

    auto has_failed() const noexcept -> bool {
        return _failed;
    }

    auto is_working() const noexcept -> bool {
        return has_started() && !(has_finished() || has_failed());
    }

    auto next(const memory::const_block input) noexcept
      -> stream_decompression& {
        if(!_started) {
            if(_compressor.decompress_begin(_method)) {
                _started = true;
            } else {
                _failed = true;
            }
        }
        if(_started) {
            if(!_compressor.decompress_next(input, _handler)) {
                _finished = true;
                _failed = !_compressor.decompress_finish();
            }
        }
        return *this;
    }

private:
    data_compressor _compressor;
    data_handler _handler;
    data_compression_method _method;
    bool _started{false};
    bool _finished{false};
    bool _failed{false};
};
//------------------------------------------------------------------------------
} // namespace eagine

