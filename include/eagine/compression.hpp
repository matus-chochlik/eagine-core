/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_COMPRESSION_HPP
#define EAGINE_COMPRESSION_HPP

#include "callable_ref.hpp"
#include "memory/block.hpp"
#include "memory/buffer.hpp"
#include <memory>

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Data compression level enumeration.
/// @ingroup main_context
/// @see data_compressor
enum class data_compression_level {
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
class data_compressor_impl;

/// @brief Class implementing data compression and decompresson.
/// @ingroup main_context
class data_compressor {
public:
    /// @brief Default constructor.
    data_compressor() noexcept;

    /// @brief Alias for data handler callable type.
    using data_handler = callable_ref<bool(memory::const_block) noexcept>;

    /// @brief Compress the input block, passing the packed data to handler.
    auto compress(
      const memory::const_block input,
      const data_handler& handler,
      const data_compression_level level) noexcept -> bool;

    /// @brief Compress the input block, writing the compressed data to output.
    auto compress(
      const memory::const_block input,
      memory::buffer& output,
      const data_compression_level level) noexcept -> memory::const_block;

    /// @brief Compress the input block, writing the compressed data to output.
    auto compress(
      const memory::const_block input,
      memory::buffer& output) noexcept -> memory::const_block {
        return compress(input, output, data_compression_level::normal);
    }

    /// @brief Compress the input block into internal buffer, returns const view.
    auto compress(
      const memory::const_block input,
      const data_compression_level level) noexcept -> memory::const_block;

    /// @brief Decompress the input block, passing the unpacked data to handler.
    auto decompress(
      const memory::const_block input,
      const data_handler& handler) noexcept -> bool;

    /// @brief Decompress the input block, writing the unpacked data to output.
    auto decompress(
      const memory::const_block input,
      memory::buffer& output) noexcept -> memory::const_block;

    /// @brief Decompress the input block into internal buffer, returns const view.
    auto decompress(const memory::const_block input) noexcept
      -> memory::const_block;

private:
    std::shared_ptr<data_compressor_impl> _pimpl{};
};
//------------------------------------------------------------------------------
} // namespace eagine

#if !EAGINE_CORE_LIBRARY || defined(EAGINE_IMPLEMENTING_CORE_LIBRARY)
#include <eagine/compression.inl>
#endif

#endif // EAGINE_COMPRESSION_HPP
