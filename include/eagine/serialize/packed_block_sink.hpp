/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_SERIALIZE_PACKED_BLOCK_SINK_HPP
#define EAGINE_SERIALIZE_PACKED_BLOCK_SINK_HPP

#include "../compression.hpp"
#include "block_sink.hpp"

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Packing serialization data sink backed by a pre-allocated memory block.
/// @ingroup serialization
/// @see packed_block_data_source
/// @see data_compressor
class packed_block_data_sink : public block_data_sink {
public:
    /// @brief Default constructor.
    packed_block_data_sink() noexcept = default;

    /// @brief Constructor setting the backing block and data compressor.
    packed_block_data_sink(
      data_compressor compressor,
      const memory::block dst) noexcept
      : block_data_sink{dst}
      , _compressor{std::move(compressor)} {}

    packed_block_data_sink(const memory::block dst) noexcept
      : packed_block_data_sink{{}, dst} {}

    packed_block_data_sink(data_compressor compressor) noexcept
      : packed_block_data_sink{std::move(compressor), {}} {}

    auto finalize() -> serialization_errors final {
        if(const auto packed{
             _compressor.compress(done(), data_compression_level::normal)}) {
            return this->replace_with(packed);
        }
        return {serialization_error_code::backend_error};
    }

private:
    data_compressor _compressor{};
};
//------------------------------------------------------------------------------
} // namespace eagine

#endif // EAGINE_SERIALIZE_PACKED_BLOCK_SINK_HPP
