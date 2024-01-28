/// @example eagine/value_tree_visitor.cpp
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

    auto input{traverse_json_stream(
      valtree::make_printing_value_tree_visitor(ctx.cio()),
      64,
      ctx.buffers(),
      ctx.log())};

    span_size_t count{1'000'000};

    input.consume_text(R"({"one_float":123.456)");
    input.consume_text(R"(,"floats":[0.0)");
    std::stringstream chunk;
    for(const auto i : integer_range(count)) {
        const auto f{i + 1};
        chunk << ", " << f << ".5";
        if(f % 256 == 0) {
            input.consume_text(chunk.str());
            chunk.str({});
        }
    }
    input.consume_text(chunk.str());
    chunk.str({});
    input.consume_text(R"(],"ints":[0)");
    for(const auto i : integer_range(count)) {
        const auto n{i + 1};
        chunk << "," << n;
        if(n % 512 == 0) {
            input.consume_text(chunk.str());
            chunk.str({});
        }
    }
    input.consume_text(chunk.str());
    input.consume_text(R"(],"one_int":789})");
    input.consume_text("J5r463/1NhsqHRGMRQQ/rxfm0jO1VcOGuBaE7XNWaSc=");
    return 0;
}

} // namespace eagine

auto main(int argc, const char** argv) -> int {
    return eagine::default_main(argc, argv, eagine::main);
}

