/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.serialization:type_compiler_info;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.build_info;
import :data_buffer;
import :read;
import :write;

namespace eagine {
//------------------------------------------------------------------------------
export template <>
struct serializer<compiler_info> : structural_serializer<compiler_info> {};

export template <>
struct deserializer<compiler_info> : structural_deserializer<compiler_info> {};
//------------------------------------------------------------------------------
} // namespace eagine

