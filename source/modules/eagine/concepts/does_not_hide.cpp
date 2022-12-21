/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.concepts:does_not_hide;

import <type_traits>;

namespace eagine {

export template <typename T, typename X>
concept does_not_hide =
  bool(not std::is_base_of_v<T, std::remove_cv_t<std::remove_reference_t<X>>>);

} // namespace eagine

