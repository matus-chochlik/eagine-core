/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_SERIALIZE_BLOCK_SOURCE_HPP
#define EAGINE_SERIALIZE_BLOCK_SOURCE_HPP

#include "../memory/block.hpp"
#include "../memory/span_algo.hpp"
#include "data_source.hpp"
#include <istream>

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Deserialization data source backed by a pre-allocated memory block.
/// @ingroup serialization
/// @see block_data_sink
/// @see packed_block_data_source
class block_data_source : public deserializer_data_source {
public:
    /// @brief Default constructor.
    block_data_source() noexcept = default;

    /// @brief Constructor setting the backing block.
    block_data_source(const memory::const_block src) noexcept
      : _src{src} {}

    /// @brief Resets the backing block.
    /// @see replace_with
    void reset(const memory::const_block src) {
        _src = src;
        _done = 0;
    }

    auto top(span_size_t req_size) -> memory::const_block final {
        return head(skip(_src, _done), req_size);
    }

    void pop(span_size_t del_size) final {
        _done += del_size;
    }

    auto remaining() const noexcept -> memory::const_block {
        return skip(_src, _done);
    }

private:
    memory::const_block _src{};
    span_size_t _done{0};
};
//------------------------------------------------------------------------------
} // namespace eagine

#endif // EAGINE_SERIALIZE_BLOCK_SOURCE_HPP
