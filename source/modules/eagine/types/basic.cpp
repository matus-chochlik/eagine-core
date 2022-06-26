/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.types:basic;

import <cstdint>;
import <type_traits>;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Byte type alias.
/// @ingroup type_utils
export using byte = unsigned char;

/// @brief Alignment type used by std.
/// @ingroup type_utils
export using std_align_t = std::size_t;

/// @brief Size type used by std.
/// @ingroup type_utils
export using std_size_t = std::size_t;

/// @brief Signed span size type used by eagine.
/// @ingroup type_utils
export using span_size_t = std::ptrdiff_t;
//------------------------------------------------------------------------------
/// @brief The underlying integer type for eagine::identifier.
/// @ingroup identifiers
/// @see identifier
export using identifier_t = std::uint64_t;

#if __SIZEOF_INT128__
/// @brief The underlying integer type for eagine::long_identifier.
/// @ingroup identifiers
/// @see long_identifier
export using long_identifier_t = __uint128_t;
#else
export using long_identifier_t = identifier_t;
#endif
//------------------------------------------------------------------------------
/// @brief Unique process identifier type (does not necessarily match to OS PID).
export using process_instance_id_t = std::uint32_t;

/// @brief Unique host identifier type.
export using host_id_t = std::uint32_t;
//------------------------------------------------------------------------------
/// @brief Alias for signed int constant type.
/// @ingroup type_utils
export template <int I>
using int_constant = std::integral_constant<int, I>;

/// @brief Alias for unsigned int constant type.
/// @ingroup type_utils
export template <unsigned U>
using unsigned_constant = std::integral_constant<unsigned, U>;

/// @brief Alias for size_t constant type.
/// @ingroup type_utils
export template <std::size_t S>
using size_constant = std::integral_constant<std::size_t, S>;

/// @brief Alias for identifier_t constant type.
/// @ingroup type_utils
/// @see identifier_t
export template <identifier_t Id>
using id_constant = std::integral_constant<identifier_t, Id>;
//------------------------------------------------------------------------------
} // namespace eagine
