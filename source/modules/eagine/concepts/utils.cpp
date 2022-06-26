/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.concepts:utils;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Implementation detail of the @c type_t template.
/// @ingroup metaprogramming
/// @note Do not use directly, use @c type_t instead.
export template <typename T, typename F>
struct get_type {
    template <typename X>
    static auto _get(X*) -> typename X::type;

    template <typename X>
    static auto _get(...) -> F;

    using type = decltype(_get<T>(static_cast<T*>(nullptr)));
};

/// @brief Template returning nested alias @c type of type @c T or @c a fallback type.
/// @ingroup metaprogramming
export template <typename T, typename F = void>
using get_type_t = typename get_type<T, F>::type;

/// @brief Template returning nested alias @c type of type @c T or @c void.
/// @ingroup metaprogramming
export template <typename T>
using type_t = typename get_type<T, void>::type;
//------------------------------------------------------------------------------
} // namespace eagine

