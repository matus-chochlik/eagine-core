/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.types;
import eagine.core.memory;
import eagine.core.reflection;
//------------------------------------------------------------------------------
namespace eagine {
//------------------------------------------------------------------------------
struct point {
    int x{}, y{}, z{};
};

template <>
struct data_member_traits<point> {
    static constexpr auto mapping() noexcept {
        return make_data_member_mapping<point, int, int, int>(
          {"x", &point::x}, {"y", &point::y}, {"z", &point::z});
    }
};
//------------------------------------------------------------------------------
struct triangle {
    point a{}, b{}, c{};
};

template <>
struct data_member_traits<triangle> {
    static constexpr auto mapping() noexcept {
        return make_data_member_mapping<triangle, point, point, point>(
          {"a", &triangle::a}, {"b", &triangle::b}, {"c", &triangle::c});
    }
};
//------------------------------------------------------------------------------
struct tetrahedron {
    triangle base{};
    point apex{};
};

template <>
struct data_member_traits<tetrahedron> {
    static constexpr auto mapping() noexcept {
        return make_data_member_mapping<tetrahedron, triangle, point>(
          {"base", &tetrahedron::base}, {"apex", &tetrahedron::apex});
    }
};
//------------------------------------------------------------------------------
} // namespace eagine
//------------------------------------------------------------------------------
// is consecutive
//------------------------------------------------------------------------------
void data_member_count_1(auto& s) {
    eagitest::case_ test{s, 1, "data member count"};

    eagine::tetrahedron th{};
    eagine::triangle tr{};
    eagine::point pt{};

    using eagine::data_member_count;
    using eagine::map_data_members;

    test.check_equal(data_member_count(map_data_members(pt)), 3, "point");
    test.check_equal(data_member_count(map_data_members(tr)), 3, "triangle");
    test.check_equal(data_member_count(map_data_members(th)), 2, "tetrahedron");
}
//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "data members", 1};
    test.once(data_member_count_1);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>

