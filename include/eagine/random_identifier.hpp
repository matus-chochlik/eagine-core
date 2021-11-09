/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#ifndef EAGINE_RANDOM_IDENTIFIER_HPP
#define EAGINE_RANDOM_IDENTIFIER_HPP

#include "identifier.hpp"

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Creates an random identifier using a default random engine.
/// @ingroup identifiers
/// @see identifier
/// @see byte_to_identifier
/// @see dec_to_identifier
auto random_identifier() -> identifier;
//------------------------------------------------------------------------------
} // namespace eagine

#if !EAGINE_CORE_LIBRARY || defined(EAGINE_IMPLEMENTING_CORE_LIBRARY)
#include <eagine/random_identifier.inl>
#endif

#endif // EAGINE_RANDOM_IDENTIFIER_HPP
