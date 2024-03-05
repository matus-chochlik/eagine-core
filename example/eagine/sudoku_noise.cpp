/// @example eagine/sudoku_noise.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
import eagine.core;
import std;

namespace eagine {

template <unsigned S>
void sudoku_noise(
  const eagine::basic_sudoku_board_traits<S> traits,
  application_config& cfg) {
    using namespace eagine;

    const auto initial = [&]() {
        if(cfg.is_set("gen_one")) {
            return traits.make_generator().generate_one();
        } else if(cfg.is_set("gen_few")) {
            return traits.make_generator().generate_few();
        } else if(cfg.is_set("gen_medium")) {
            return traits.make_generator().generate_medium();
        } else if(cfg.is_set("gen_many")) {
            return traits.make_generator().generate_many();
        } else {
            return traits.make_diagonal();
        }
    };

    const auto width = cfg.get<int>("width").value_or(16);
    const auto height = cfg.get<int>("height").value_or(8);

    basic_sudoku_tiling<S> bst{traits, initial()};
    bst.print(std::cout, width, height);
}

auto main(main_ctx& ctx) -> int {
    auto& cfg = ctx.config();

    const auto rank = cfg.get<int>("rank").value_or(4);

    switch(rank) {
        case 3: {
            // clang-format off
			sudoku_noise<3>({
				{" ","▀","▄","▖","▗","▘","▝","▚","▞"},
				{"?","?","?"}, " "
			}, cfg);
            // clang-format on
            break;
        }
        case 4: {
            // clang-format off
			sudoku_noise<4>({
				{" ","▀","▄","█","▌","▐","▖","▗","▘","▙","▚","▛","▜","▝","▞","▟"},
				{"?","?","?","?"}, " "
			}, cfg);
            // clang-format on
            break;
        }
        default:
            ctx.log().error("invalid rank value: ${rank}").arg("rank", rank);
            break;
    }

    return 0;
}
} // namespace eagine

auto main(int argc, const char** argv) -> int {
    return eagine::default_main(argc, argv, eagine::main);
}

