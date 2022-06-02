/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.types:selector;

import :basic;

namespace eagine {

/// @brief Template used to construct tag-types used mostly in tag-dispatching.
/// @ingroup metaprogramming
/// @see default_selector
/// @see construct_from
export template <identifier_t V>
struct selector {
    static constexpr const identifier_t value = V;
    using type = selector;
};

/// @brief Type alias for the default overload selector type.
/// @ingroup metaprogramming
/// @see default_selector
export using default_selector_t = selector<0U>;

/// @brief The default overload selector constant.
/// @ingroup metaprogramming
/// @see default_selector_t
export constexpr const default_selector_t default_selector{};

/// @brief Type alias for the construct-from tag-dispatch selector.
/// @ingroup metaprogramming
/// @see construct_from
///
/// This type is typically used as an additional parameter in templated
/// constructors to avoid conflicts with copy/move constructors.
export using construct_from_t = selector<0xFU>;

/// @brief The construct-from tag-dispatch constant.
/// @ingroup metaprogramming
/// @see construct_from_t
export constexpr const construct_from_t construct_from{};

/// @brief Type alias for immediate function selector.
/// @ingroup metaprogramming
/// @see immediate_function
///
/// This type is typically used as an additional parameter in consteval
/// function overloads.
export using immediate_function_t = selector<0x1FU>;

/// @brief The use immediate function tag-dispatch constant
/// @ingroup metaprogramming
/// @see immediate_function_t
export constexpr const immediate_function_t immediate_function = {};

} // namespace eagine
