/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.types:nothing;

namespace eagine {

/// @brief Class representing "none" / "nothing" values.
/// @ingroup type_utils
/// @see nothing
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

} // namespace eagine
