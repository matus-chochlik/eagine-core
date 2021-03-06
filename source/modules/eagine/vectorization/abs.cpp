/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.vectorization:abs;

import :data;
import <cmath>;

namespace eagine::vect {

template <typename T, int N, bool V>
struct abs {
    static auto apply(data_t<T, N, V> v) noexcept -> data_t<T, N, V> {
        for(int i = 0; i < N; ++i) {
            using std::abs;
            v[i] = abs(v[i]);
        }
        return v;
    }
};

} // namespace eagine::vect
