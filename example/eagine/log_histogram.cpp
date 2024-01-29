/// @example eagine/log_histogram.cpp
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
auto main(main_ctx& ctx) -> int {
    static const auto exec_time_id{ctx.log().register_time_interval("main")};
    const auto exec_time{ctx.log().measure_time_interval(exec_time_id)};

    file_contents content_of_self{ctx.exe_path()};
    memory::const_block data{content_of_self};

    std::array<span_size_t, 256> byte_counts{};
    zero(cover(byte_counts));

    span_size_t max_count{0};
    for(const auto b : data) {
        max_count = math::maximum(max_count, ++byte_counts[std_size(b)]);
    }

    ctx.log()
      .info("byte histogram")
      .arg_func([max_count, &byte_counts](logger_backend& backend) {
          for(const auto i : integer_range(std_size(256))) {
              backend.add_float(
                byte_to_identifier(byte(i)),
                "Histogram",
                float(0),
                float(byte_counts[i]),
                float(max_count));
          }
      });

    return 0;
}
//------------------------------------------------------------------------------
} // namespace eagine

auto main(int argc, const char** argv) -> int {
    return eagine::default_main(argc, argv, eagine::main);
}

