/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.vectorization:view;

import eagine.core.memory;
import :data;
import <cmath>;
import <type_traits>;

namespace eagine::vect {

export template <typename T, int N, bool V>
struct view {
private:
    static auto _addr(const data_t<T, N, V>& d, const std::false_type) noexcept
      -> const T* {
        return static_cast<const T*>(d._v);
    }

    static auto _addr(const data_t<T, N, V>& d, const std::true_type) noexcept
      -> const T* {
        return reinterpret_cast<const T*>(&d);
    }

public:
    static auto apply(const data_t<T, N, V>& d) noexcept -> span<const T> {
        static_assert(sizeof(T[N]) == sizeof(data_t<T, N, V>));
        return {_addr(d, has_simd_data<T, N, V>()), N};
    }

    template <int M>
    static auto apply(const data_t<T, N, V> (&d)[M]) noexcept -> span<const T> {
        static_assert(sizeof(T[N][M]) == sizeof(data_t<T, N, V>[M]));
        return {_addr(d[0], has_simd_data<T, N, V>()), N * M};
    }
};

} // namespace eagine::vect

