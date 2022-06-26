/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.concepts:extractable;

import <concepts>;
import <type_traits>;
import <utility>;

namespace eagine {

export template <typename T>
struct extract_traits {};

export template <typename T>
constexpr auto has_value(T&& xtr) noexcept -> bool {
    return bool(std::forward<T>(xtr));
}

export template <typename T>
constexpr auto extract(T&& xtr) noexcept -> decltype(auto) {
    return *std::forward<T>(xtr);
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
concept extractable = basic_extractable<T> && requires(T v) {
    { has_value(v) } -> std::convertible_to<bool>;
};
// clang-format on
//------------------------------------------------------------------------------
} // namespace eagine
