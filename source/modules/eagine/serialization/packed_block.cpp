/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.serialization:packed_block;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.runtime;
import :result;
import :block;
import <utility>;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Packing serialization data sink backed by a pre-allocated memory block.
/// @ingroup serialization
/// @see packed_block_data_source
/// @see data_compressor
export class packed_block_data_sink : public block_data_sink {
public:
    /// @brief Constructor setting the backing block and data compressor.
    packed_block_data_sink(
      data_compressor compressor,
      const memory::block dst) noexcept
      : block_data_sink{dst}
      , _compressor{std::move(compressor)} {}

    packed_block_data_sink(data_compressor compressor) noexcept
      : packed_block_data_sink{std::move(compressor), {}} {}

    auto finalize() noexcept -> serialization_errors final {
        if(const auto packed{_compressor.default_compress(
             done(), data_compression_level::normal)}) {
            return this->replace_with(packed);
        }
        return {serialization_error_code::backend_error};
    }

private:
    data_compressor _compressor;
};
//------------------------------------------------------------------------------
/// @brief Unpacking deserialization data source backed by a pre-allocated memory block.
/// @ingroup serialization
/// @see packed_block_data_sink
/// @see data_compressor
export class packed_block_data_source : public block_data_source {
public:
    /// @brief Constructor setting the backing block and data compressor.
    packed_block_data_source(
      data_compressor compressor,
      const memory::const_block src) noexcept
      : _compressor{std::move(compressor)} {
        reset(src);
    }

    packed_block_data_source(data_compressor compressor) noexcept
      : packed_block_data_source{std::move(compressor), {}} {}

    void reset(const memory::const_block src) {
        block_data_source::reset(_compressor.default_decompress(src));
    }

private:
    data_compressor _compressor;
};
//------------------------------------------------------------------------------
} // namespace eagine

