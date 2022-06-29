/// @example eagine/value_tree.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#if EAGINE_CORE_MODULE
import eagine.core;
import <memory>;
#else
#include <eagine/logging/logger.hpp>
#include <eagine/main_ctx.hpp>
#include <eagine/value_tree/empty.hpp>
#include <eagine/value_tree/filesystem.hpp>
#include <eagine/value_tree/json.hpp>
#include <eagine/value_tree/yaml.hpp>
#include <memory>
#endif

namespace eagine {

auto main(main_ctx& ctx) -> int {
    const auto& log = ctx.log();
    const string_view n_a{"N/A"};

    const auto path = [](string_view str) {
        return basic_string_path(str, split_by, "/");
    };

    const auto visitor = [&ctx](
                           const valtree::compound& c,
                           const valtree::attribute& a,
                           const basic_string_path& p) {
        auto ca{c / a};
        ctx.log()
          .info("visit")
          .arg(identifier{"nested"}, ca.nested_count())
          .arg(identifier{"values"}, ca.value_count())
          .arg(identifier{"isLink"}, identifier{"bool"}, ca.is_link())
          .arg(identifier{"canonType"}, ca.canonical_type())
          .arg(identifier{"path"}, p.as_string("/", ca.nested_count() > 0))
          .arg(identifier{"name"}, ca.name());

        if(ca.canonical_type() == valtree::value_type::byte_type) {
            const auto s{ca.value_count()};
            if(s <= 256) {
                std::array<byte, 256> temp{};
                auto content{ca.fetch_blob(cover(temp))};
                ctx.log().info("content").arg(
                  identifier{"content"}, view(content));
            }
        } else if(ca.canonical_type() == valtree::value_type::string_type) {
            if(ca.value_count() == 1) {
                std::array<char, 64> temp{};
                auto content{ca.fetch_values(cover(temp))};
                ctx.log().info("content").arg(
                  identifier{"content"}, string_view(content));
            }
        }
        return true;
    };

    const string_view json_text(R"({
		"attribA" : {
			"attribB": 123
		},
		"attribC" : [
			45, "six", 78.9, {"zero": false}
		],
		"attribD" : "VGhpcyBpcyBhIGJhc2U2NC1lbmNvZGVkIEJMT0IK"
	})");

    if(const auto json_tree{valtree::from_json_text(json_text, ctx.log())}) {
        std::array<byte, 64> temp{};
        log.info("parsed from json")
          .arg(
            identifier{"attribB"},
            identifier{"int"},
            json_tree.get<int>(path("attribA/attribB")),
            n_a)
          .arg(
            identifier{"attribC0"},
            identifier{"int"},
            json_tree.get<int>(path("attribC/0")),
            n_a)
          .arg(
            identifier{"attribC1"},
            identifier{"string"},
            json_tree.get<std::string>(path("attribC/1")),
            n_a)
          .arg(
            identifier{"attribC2"},
            identifier{"float"},
            json_tree.get<float>(path("attribC/2")),
            n_a)
          .arg(
            identifier{"attribC3z"},
            identifier{"bool"},
            json_tree.get<bool>(path("attribC/3/zero")),
            n_a)
          .arg(
            identifier{"attribD"},
            view(json_tree.fetch_blob(path("attribD"), cover(temp))));
        json_tree.traverse(
          valtree::compound::visit_handler{construct_from, visitor});
    }

    const string_view yaml_text(
      "attribA:\n"
      "    attribB: 123\n"
      "attribC: [45, six, 78.9, zero: false]\n");

    if(const auto yaml_tree{valtree::from_yaml_text(yaml_text, ctx.log())}) {
        log.info("parsed from yaml")
          .arg(
            identifier{"attribB"},
            identifier{"int"},
            yaml_tree.get<int>(path("attribA/attribB")),
            n_a)
          .arg(
            identifier{"attribC0"},
            identifier{"int"},
            yaml_tree.get<int>(path("attribC/0")),
            n_a)
          .arg(
            identifier{"attribC1"},
            identifier{"string"},
            yaml_tree.get<std::string>(path("attribC/1")),
            n_a)
          .arg(
            identifier{"attribC2"},
            identifier{"float"},
            yaml_tree.get<float>(path("attribC/2")),
            n_a)
          .arg(
            identifier{"attribC3z"},
            identifier{"bool"},
            yaml_tree.get<bool>(path("attribC/3/zero")),
            n_a);
        yaml_tree.traverse(
          valtree::compound::visit_handler{construct_from, visitor});
    }

    if(const auto path_arg{ctx.args().find("--fs-tree").next()}) {
        log.info("opening ${root} filesystem tree")
          .arg(identifier{"root"}, path_arg.get());
        if(const auto fs_tree{
             valtree::from_filesystem_path(path_arg, ctx.log())}) {
            fs_tree.traverse(
              valtree::compound::visit_handler{construct_from, visitor});
        }
    }

    if(const auto empty_tree{valtree::empty(ctx.log())}) {
        empty_tree.traverse(
          valtree::compound::visit_handler{construct_from, visitor});
    }

    return 0;
}

} // namespace eagine

auto main(int argc, const char** argv) -> int {
    return eagine::default_main(argc, argv, eagine::main);
}

