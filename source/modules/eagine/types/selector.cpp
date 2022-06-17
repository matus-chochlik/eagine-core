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
export constexpr const immediate_function_t immediate_function{};

/// @brief Type alias for split-by function selector.
/// @ingroup metaprogramming
/// @see split_by
///
/// This type is typically used as an additional parameter some string function
/// function overloads.
export using split_by_t = selector<0x58U>;

/// @brief The split-by function tag-dispatch constant
/// @ingroup metaprogramming
/// @see split_by_t
export constexpr const split_by_t split_by{};

/// @brief Type alias for from-pack function selector.
/// @ingroup metaprogramming
/// @see from_pack
///
/// This type is typically used as an additional parameter some string function
/// overloads.
export using from_pack_t = selector<0xF9U>;

/// @brief The from-pack function tag-dispatch constant
/// @ingroup metaprogramming
/// @see from_pack_t
export constexpr const from_pack_t from_pack{};

/// @brief Type alias for application-config function selector.
/// @ingroup metaprogramming
/// @see from_config
///
/// This type is typically used as an additional parameter some function
/// overloads.
export using from_config_t = selector<0xACU>;

/// @brief The from-config function tag-dispatch constant
/// @ingroup metaprogramming
/// @see from_config_t
export constexpr const from_config_t from_config{};

/// @brief Type alias for from-value-tree function selector.
/// @ingroup metaprogramming
/// @see from_value_tree
///
/// This type is typically used as an additional parameter some function
/// overloads.
export using from_value_tree_t = selector<0xF7U>;

/// @brief The from-config function tag-dispatch constant
/// @ingroup metaprogramming
/// @see from_config_t
export constexpr const from_value_tree_t from_value_tree{};

} // namespace eagine
