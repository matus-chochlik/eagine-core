/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.identifier:construct;

import eagine.core.types;
export import :decl;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Creates an identifier representing a byte value as a hex string.
/// @ingroup identifiers
/// @see identifier
/// @see dec_to_identifier
/// @see random_identifier
export auto byte_to_identifier(const byte b) noexcept -> identifier;

/// @brief Creates an identifier representing an integer value as a decimal string.
/// @ingroup identifiers
/// @see identifier
/// @see byte_to_identifier
/// @see random_identifier
export auto dec_to_identifier(const int i) noexcept -> identifier;
//------------------------------------------------------------------------------
} // namespace eagine

