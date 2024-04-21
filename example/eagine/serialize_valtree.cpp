/// @example eagine/serialize_valtree.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
import eagine.core;
import std;

namespace eagine {
//------------------------------------------------------------------------------
struct point {
    float x{}, y{}, z{};
};

template <>
struct data_member_traits<point> {
    static constexpr auto mapping() noexcept {
        return make_data_member_mapping<point, float, float, float>(
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
auto main(main_ctx& ctx) -> int {

    const string_view json_text(R"({
		"base" : {
			"a": {"x": 0.0, "y": 0.0, "z": 0.0},
			"b": {"x": 1.0, "y": 0.0, "z": 0.0},
			"c": {"x": 0.0, "y": 1.0, "z": 0.0}
		},
		"apex": {"x": 0.0, "y": 0.0, "z": 1.0}
	})");

    if(const auto json_tree{valtree::from_json_text(json_text, ctx)}) {
        valtree_deserializer_backend backend{json_tree};

        tetrahedron object;
        if(deserialize(object, backend)) {
            main_ctx_object out{"vt", ctx};
            out
              .cio_print(
                "("
                "(${b_a_x},${b_a_y},${b_a_z}),"
                "(${b_b_x},${b_b_y},${b_b_z}),"
                "(${b_c_x},${b_c_y},${b_c_z})),"
                "(${a_x},${a_y},${a_z})")
              .arg("b_a_x", object.base.a.x)
              .arg("b_a_y", object.base.a.y)
              .arg("b_a_z", object.base.a.z)
              .arg("b_b_x", object.base.b.x)
              .arg("b_b_y", object.base.b.y)
              .arg("b_b_z", object.base.b.z)
              .arg("b_c_x", object.base.c.x)
              .arg("b_c_y", object.base.c.y)
              .arg("b_c_z", object.base.c.z)
              .arg("a_x", object.apex.x)
              .arg("a_y", object.apex.y)
              .arg("a_z", object.apex.z);
        }
    }

    return 0;
}
//------------------------------------------------------------------------------
} // namespace eagine

auto main(int argc, const char** argv) -> int {
    return eagine::default_main(argc, argv, eagine::main);
}

