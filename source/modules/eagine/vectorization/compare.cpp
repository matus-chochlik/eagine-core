/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.vectorization:compare;

import :data;

namespace eagine::vect {

export template <typename T, int N, bool V>
struct is_zero {
    [[nodiscard]] static auto apply(data_param_t<T, N, V> v) noexcept -> bool {
        bool result = true;
        for(int i = 0; i < N; ++i) {
            result &= not(v[i] > T(0) or v[i] < T(0));
        }
        return result;
    }
};

// TODO: some optimizations ?

} // namespace eagine::vect
