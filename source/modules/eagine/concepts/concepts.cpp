/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.concepts;

import <concepts>;
import <type_traits>;
import <utility>;

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
// extractable
//------------------------------------------------------------------------------
export template <typename T>
struct extract_traits {};

export template <typename T>
constexpr auto has_value(T&& xtr) noexcept -> bool {
    return bool(std::forward<T>(xtr));
}

/// @brief Returns the value type of an extractable.
/// @ingroup utilities
/// @see extract
export template <typename T>
using extracted_type_t = std::remove_cv_t<typename extract_traits<
  std::remove_cv_t<std::remove_reference_t<T>>>::value_type>;

export template <typename T>
using extract_result_type_t = typename extract_traits<
  std::remove_cv_t<std::remove_reference_t<T>>>::result_type;

export template <typename T>
using const_extract_result_type_t = typename extract_traits<
  std::remove_cv_t<std::remove_reference_t<T>>>::const_result_type;

export template <typename E, typename V>
constinit const auto has_value_type_v =
  std::is_convertible_v<extracted_type_t<E>, V>;
//------------------------------------------------------------------------------
// clang-format off
export template <typename T>
concept basic_extractable = requires(T v) {
	{ std::declval<eagine::extracted_type_t<T>>() };
	{ std::declval<eagine::extract_result_type_t<T>>() };
	extract(v);
};

export template <typename T>
concept extractable = basic_extractable<T> and requires(T v) {
    { has_value(v) } -> std::convertible_to<bool>;
};
// clang-format on
//------------------------------------------------------------------------------
// optional_like
//------------------------------------------------------------------------------
export template <typename T>
concept optional_like = requires(T v) {
    v.value();
    v.value_or(v.value());
    v.transform([](auto x) { return x; });
    { v.has_value() } -> std::convertible_to<bool>;
};
//------------------------------------------------------------------------------
// does not hide
//------------------------------------------------------------------------------
export template <typename T, typename X>
concept does_not_hide =
  bool(not std::is_base_of_v<T, std::remove_cv_t<std::remove_reference_t<X>>>);
//------------------------------------------------------------------------------
} // namespace eagine

