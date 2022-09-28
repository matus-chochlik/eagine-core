/// @example eagine/value_tree_visitor.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
import eagine.core;
import <memory>;

import <iostream>;
namespace eagine {

class printing_value_tree_visitor
  : public main_ctx_object
  , public valtree::value_tree_visitor {
public:
    printing_value_tree_visitor(main_ctx_parent parent)
      : main_ctx_object{"PrnVTVistr", parent} {}

    void begin() final {
        cio_print("begin traversal");
    }

    void consume(span<const nothing_t> data) final {
        cio_print("consume nil: ${count] instances").arg("count", data.size());
    }

    template <typename T>
    void _consume(span<const T> data) {
        const auto value_cio{cio_print("consume:").to_be_continued()};

        for(const auto& value : data) {
            value_cio.print("'${value}'").arg("value", value);
        }
    }

    void consume(span<const bool> data) final {
        _consume(data);
    }
    void consume(span<const std::int64_t> data) final {
        _consume(data);
    }
    void consume(span<const std::uint64_t> data) final {
        _consume(data);
    }
    void consume(span<const float> data) final {
        _consume(data);
    }
    void consume(span<const double> data) final {
        _consume(data);
    }
    void consume(span<const string_view> data) final {
        _consume(data);
    }

    void begin_struct() final {
        cio_print("enter structure");
    }

    void begin_attribute(const string_view name) final {
        cio_print("enter attribute: ${name}").arg("name", name);
    }

    void finish_attribute(const string_view name) final {
        cio_print("leave attribute: ${name}").arg("name", name);
    }

    void finish_struct() final {
        cio_print("leave structure");
    }

    void begin_list() final {
        cio_print("enter list");
    }

    void finish_list() final {
        cio_print("leave list");
    }

    void finish() final {
        cio_print("finish traversal");
    }
};

auto main(main_ctx& ctx) -> int {
    const string_view json_text(R"({
		"attribA" : {
			"attribB": 123
		},
		"attribC" : [
			45, "six", 78.9, {"zero": false}
		],
		"attribD" : "VGhpcyBpcyBhIGJhc2U2NC1lbmNvZGVkIEJMT0IK"
	})");

    traverse_json_stream(
      std::make_shared<printing_value_tree_visitor>(ctx), ctx.log())
      .consume_text(json_text);

    return 0;
}

} // namespace eagine

auto main(int argc, const char** argv) -> int {
    return eagine::default_main(argc, argv, eagine::main);
}

