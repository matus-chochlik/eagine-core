/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_EXTRACTABLE_HPP
#define EAGINE_EXTRACTABLE_HPP

#include "nothing.hpp"
#include <type_traits>

namespace eagine {

template <typename T>
struct extract_traits;

/// @brief Returns the value type of an extractable.
/// @ingroup utilities
/// @see extract
template <typename T>
using extracted_type_t = std::remove_cv_t<typename extract_traits<
  std::remove_cv_t<std::remove_reference_t<T>>>::value_type>;

template <typename T>
using extract_result_type_t = typename extract_traits<
  std::remove_cv_t<std::remove_reference_t<T>>>::result_type;

template <typename T>
using const_extract_result_type_t = typename extract_traits<
  std::remove_cv_t<std::remove_reference_t<T>>>::const_result_type;

template <typename E, typename V>
static constinit const auto has_value_type_v =
  std::is_convertible_v<extracted_type_t<E>, V>;
//------------------------------------------------------------------------------
// clang-format off
template <typename T>
concept basic_extractable = requires(T v) {
	{ std::declval<eagine::extracted_type_t<T>>() };
	{ std::declval<eagine::extract_result_type_t<T>>() };
	extract(v);
};

template <typename T>
concept extractable = basic_extractable<T> && requires(T v) {
    { has_value(v) } -> std::convertible_to<bool>;
};
// clang-format on
//------------------------------------------------------------------------------
} // namespace eagine

#endif
