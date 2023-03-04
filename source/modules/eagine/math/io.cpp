/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.math:io;

import :primitives;
import :vector;
import :matrix;
import std;

namespace eagine::math {
//------------------------------------------------------------------------------
/// @brief Operator for writing vector into output stream.
/// @ingroup math
export template <typename T, int N, bool V>
auto operator<<(std::ostream& o, const vector<T, N, V>& v) -> std::ostream& {

    o << "[" << v._v[0];
    for(int i = 1; i < N; ++i) {
        o << ", " << v._v[i];
    }
    return o << "]";
}
//------------------------------------------------------------------------------
/// @brief Operator for writing row-major matrix into output stream.
/// @ingroup math
export template <typename T, int C, int R, bool V>
auto operator<<(std::ostream& o, const matrix<T, C, R, true, V>& m)
  -> std::ostream& {
    o << "|R0[" << m._v[0][0];

    for(int c = 1; c < C; ++c) {
        o << ", " << m._v[0][c];
    }
    o << "]";

    for(int r = 1; r < R; ++r) {
        o << ",R" << r << "[" << m._v[r][0];
        for(int c = 1; c < C; ++c) {
            o << ", " << m._v[r][c];
        }
        o << "]";
    }
    return o << "|";
}
//------------------------------------------------------------------------------
/// @brief Operator for writing column-major matrix into output stream.
/// @ingroup math
export template <typename T, int C, int R, bool V>
auto operator<<(std::ostream& o, const matrix<T, C, R, false, V>& m)
  -> std::ostream& {
    o << "|C0[" << m._v[0][0];

    for(int r = 1; r < R; ++r) {
        o << ", " << m._v[0][r];
    }
    o << "]";

    for(int c = 1; c < C; ++c) {
        o << ",C" << c << "[" << m._v[c][0];
        for(int r = 1; r < R; ++r) {
            o << ", " << m._v[c][r];
        }
        o << "]";
    }
    return o << "|";
}
//------------------------------------------------------------------------------
/// @brief Operator for writing line into output stream.
/// @ingroup math
export template <typename T, int N, bool V>
auto operator<<(std::ostream& o, const basic_line<T, N, V>& l)
  -> std::ostream& {
    o << '[' << l.origin() << " -> " << l.direction() << ']';
    return o;
}
//------------------------------------------------------------------------------
/// @brief Operator for writing triangle into output stream.
/// @ingroup math
export template <typename T, int N, bool V>
auto operator<<(std::ostream& o, const basic_triangle<T, N, V>& t)
  -> std::ostream& {
    o << '[' << t.a() << '|' << t.b() << '|' << t.c() << ']';
    return o;
}
//------------------------------------------------------------------------------
/// @brief Operator for writing sphere into output stream.
/// @ingroup math
export template <typename T, bool V>
auto operator<<(std::ostream& o, const sphere<T, V>& s) -> std::ostream& {
    o << '[' << s.center() << ')' << s.radius() << ']';
    return o;
}
//------------------------------------------------------------------------------
} // namespace eagine::math
