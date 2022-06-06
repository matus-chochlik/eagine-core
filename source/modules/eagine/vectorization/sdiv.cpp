/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.vectorization:sdiv;

import eagine.core.types;
import :data;
import <type_traits>;

namespace eagine::vect {

export template <typename T, int N, bool V>
struct sdiv {
    static constexpr auto apply(
      data_param_t<T, N, V> a,
      data_param_t<T, N, V> b) noexcept -> data_t<T, N, V> {
        return a / b;
    }
};

#if defined(__GNUC__) || defined(__clang__)

export template <typename T, bool V>
struct sdiv<T, 3, V> {
    static constexpr auto _hlp(
      data_param_t<T, 3, V> a,
      data_param_t<T, 3, V> b,
      const std::true_type) noexcept -> data_t<T, 3, V> {
        return a / data_t<T, 3, V>{b[0], b[1], b[2], T(1)};
    }

    static constexpr auto _hlp(
      data_param_t<T, 3, V> a,
      data_param_t<T, 3, V> b,
      const std::false_type) noexcept -> data_t<T, 3, V> {
        return a / b;
    }

    static constexpr auto apply(
      data_param_t<T, 3, V> a,
      data_param_t<T, 3, V> b) noexcept -> data_t<T, 3, V> {
        return _hlp(a, b, has_vect_data<T, 3, V>());
    }
};

#endif

} // namespace eagine::vect

