/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.serialization:type_version_info;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.build_info;
import :data_buffer;
import :read;
import :write;

namespace eagine {
//------------------------------------------------------------------------------
template <>
struct serializer<version_info> : structural_serializer<version_info> {};

template <>
struct deserializer<version_info> : structural_deserializer<version_info> {};
//------------------------------------------------------------------------------
} // namespace eagine

