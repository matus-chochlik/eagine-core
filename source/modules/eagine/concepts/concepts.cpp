/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.concepts;

import std;

namespace eagine {
//------------------------------------------------------------------------------
// utils
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
// optional_like
//------------------------------------------------------------------------------
export template <typename T>
struct extract_traits {
    using value_type = typename T::value_type;
};

/// @brief Returns the value type of an extractable.
/// @ingroup utilities
/// @see extract
export template <typename T>
using extracted_type_t =
  std::remove_cv_t<typename extract_traits<std::remove_cvref_t<T>>::value_type>;

export template <typename E, typename V>
constinit const auto has_value_type_v =
  std::is_convertible_v<extracted_type_t<E>, V>;
//------------------------------------------------------------------------------
export template <typename V>
concept optional_like = requires(V v) {
    v.transform([]<typename T>(T&& x) -> std::size_t {
        return sizeof(std::forward<T>(x));
    });
    v.and_then([]<typename T>(T&& x) -> std::optional<std::size_t> {
        return {sizeof(std::forward<T>(x))};
    });
    static_cast<bool>(v);
    v.value();
    v.value_or(v.value());
    { v.has_value() } -> std::convertible_to<bool>;
    { *v } -> std::convertible_to<const extracted_type_t<V>&>;
};

export template <typename T, typename V>
concept extracts_to =
  optional_like<T> and std::is_convertible_v<extracted_type_t<T>, V>;
//------------------------------------------------------------------------------
// does not hide
//------------------------------------------------------------------------------
export template <typename T, typename X>
concept does_not_hide =
  bool(not std::is_base_of_v<T, std::remove_cv_t<std::remove_reference_t<X>>>);
//------------------------------------------------------------------------------
} // namespace eagine

