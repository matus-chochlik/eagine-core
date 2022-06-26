/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.types:nothing;
import eagine.core.build_config;
import <type_traits>;

namespace eagine {

/// @brief Class representing "none" / "nothing" values.
/// @ingroup type_utils
/// @see nothing
/// @see not_in_low_profile
export struct nothing_t {

    /// @brief Type alias to self.
    using type = nothing_t;

    /// @brief Constructible from any number and types of arguments.
    template <typename... T>
    constexpr nothing_t(const T...) noexcept {}
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

} // namespace eagine
