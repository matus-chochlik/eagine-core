/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_MEMORY_COPY_HPP
#define EAGINE_MEMORY_COPY_HPP

#include "../assert.hpp"
#include "block.hpp"
#include "buffer.hpp"
#include <cstring>

namespace eagine::memory {

/// @brief Copies the content of source block to destination block.
/// @ingroup memory
/// @see const_block
/// @see block
static inline auto copy(const const_block source, block dest) noexcept
  -> block {
    EAGINE_ASSERT(dest.size() >= source.size());
    std::memcpy(dest.data(), source.data(), integer(source.size()));
    return {dest.data(), source.size()};
}

/// @brief Copies the content of source block to destination buffer.
/// @ingroup memory
/// @see const_block
/// @see buffer
static inline auto copy_into(const const_block source, buffer& dest) noexcept
  -> block {
    return copy(source, cover(dest.resize(source.size())));
}

} // namespace eagine::memory

#endif // EAGINE_MEMORY_COPY_HPP
