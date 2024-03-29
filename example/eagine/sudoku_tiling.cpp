/// @example eagine/sudoku_tiling.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
import eagine.core;
import std;

template <unsigned S>
void sudoku_tiling(eagine::span_size_t w, eagine::span_size_t h) {
    using namespace eagine;
    const default_sudoku_board_traits<S> traits;

    const auto print_generated{[&](int x, int y, const auto& board) {
        basic_sudoku_tile_patch<S> patch(w, h);
        basic_sudoku_tiling<S> bst{traits, board};
        std::cout << bst.fill(x, y, patch) << std::endl;
    }};

    const auto board{traits.make_generator().generate_medium()};
    print_generated(-1, -1, board);
    print_generated(1, 1, board);
}

auto main(int argc, const char** argv) -> int {
    eagine::program_args args{argc, argv};

    sudoku_tiling<4>(
      from_string<int>(args.find("--width")).value_or(32),
      from_string<int>(args.find("--height")).value_or(24));

    return 0;
}
