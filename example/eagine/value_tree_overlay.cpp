/// @example eagine/value_tree_overlay.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
import eagine.core;
import std;

namespace eagine {

auto main(main_ctx& ctx) -> int {
    auto overlay = valtree::make_overlay(ctx);

    const string_view json_text(R"({
		"attribA" : {
			"attribB": 123
		},
		"attribC" : [
			45, "six", 78.9, {"zero": false}
		],
		"attribD" : "VGhpcyBpcyBhIGJhc2U2NC1lbmNvZGVkIEJMT0IK"
	})");

    valtree::add_overlay(overlay, valtree::from_json_text(json_text, ctx));

    const string_view yaml_text(
      "attribA:\n"
      "    attribB: 1234\n"
      "attribC: [56, seven, 8.9]\n");

    valtree::add_overlay(overlay, valtree::from_yaml_text(yaml_text, ctx));

    const auto visitor = [&ctx](
                           const valtree::compound& c,
                           const valtree::attribute& a,
                           const basic_string_path& p) {
        auto ca{c / a};
        ctx.log()
          .info("visit")
          .arg("nested", ca.nested_count())
          .arg("values", ca.value_count())
          .arg("isLink", "bool", ca.is_link())
          .arg("canonType", ca.canonical_type())
          .arg("path", p.as_string("/", ca.nested_count() > 0))
          .arg("name", ca.name());

        if(ca.canonical_type() == valtree::value_type::byte_type) {
            const auto s{ca.value_count()};
            if(s <= 256) {
                std::array<byte, 256> temp{};
                auto content{ca.fetch_blob(cover(temp))};
                ctx.log().info("content").arg("content", view(content));
            }
        } else if(ca.canonical_type() == valtree::value_type::string_type) {
            if(ca.value_count() == 1) {
                std::array<char, 64> temp{};
                auto content{ca.fetch_values(cover(temp))};
                ctx.log().info("content").arg("content", string_view(content));
            }
        }
        return true;
    };

    overlay.traverse(valtree::compound::visit_handler{construct_from, visitor});

    return true;
}

} // namespace eagine

auto main(int argc, const char** argv) -> int {
    return eagine::default_main(argc, argv, eagine::main);
}

