/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.vectorization:diff;

import :data;
import :abs;

namespace eagine::vect {

export template <typename T, int N, bool V>
struct diff {
    static auto apply(data_param_t<T, N, V> a, data_param_t<T, N, V> b) noexcept
      -> data_t<T, N, V> {
        return vect::abs<T, N, V>::apply(a - b);
    }
};

} // namespace eagine::vect
