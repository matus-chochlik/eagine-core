/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.memory;

import std;
import eagine.core.types;
export import :align;
export import :address;
export import :bit_density;
export import :null_ptr;
export import :parent_ptr;
export import :offset_ptr;
export import :span;
export import :offset_span;
export import :string_span;
export import :span_algorithm;
export import :split_span;
export import :aligned_block;
export import :byteset;
export import :biteset;
export import :fixed_size_string;
export import :flatten;
export import :edit_distance;
export import :byte_allocator;
export import :stack_allocator;
export import :std_allocator;
export import :buffer;
export import :object_storage;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Converts a hexadecimal digit character to byte.
export constexpr auto hex_char2byte(const char c) noexcept
  -> std::optional<byte> {
    if(('0' <= c) and (c <= '9')) {
        return byte(c - '0');
    } else if(('a' <= c) and (c <= 'f')) {
        return byte(10U + (c - 'a'));
    } else if(('A' <= c) and (c <= 'F')) {
        return byte(10U + (c - 'A'));
    }
    return {};
}
//------------------------------------------------------------------------------
} // namespace eagine
