/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.types:basic;

import std;
import eagine.core.build_config;

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
/// @brief Type that can by constructed from single argument of any other type.
/// @ingroup type_utils
export struct anything {
    /// @brief Default constructor.
    constexpr anything() noexcept = default;

    /// @brief Move constructor.
    constexpr anything(anything&&) noexcept = default;

    /// @brief Copy constructor.
    constexpr anything(const anything&) noexcept = default;

    /// @brief Move assignment operator.
    constexpr auto operator=(anything&&) noexcept -> anything& = default;

    /// @brief Copy assignment operator.
    constexpr auto operator=(const anything&) noexcept -> anything& = default;

    ~anything() noexcept = default;

    /// @brief Construction from any other type.
    template <typename Unused>
    constexpr anything(const Unused&) noexcept {}
};
//------------------------------------------------------------------------------
/// @brief Class representing "none" / "nothing" values.
/// @ingroup type_utils
/// @see nothing
/// @see not_in_low_profile
export struct nothing_t {

    /// @brief Type alias to self.
    using type = nothing_t;
};

/// @brief Constant of nothing_t type.
/// @ingroup type_utils
/// @see nothing_t
export constexpr nothing_t nothing{};

/// @brief Alias that results in T in non low-profile builds and in nothing_t otherwise.
/// @ingroup type_utils
/// @see nothing_t
export template <typename T>
using not_in_low_profile = std::conditional_t<low_profile_build, nothing_t, T>;
//------------------------------------------------------------------------------
} // namespace eagine
