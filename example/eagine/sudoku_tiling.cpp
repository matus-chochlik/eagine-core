/// @example eagine/sudoku_tiling.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
import eagine.core;
import std;

template <unsigned S>
void sudoku_tiling(eagine::span_size_t w, eagine::span_size_t h) {
    using namespace eagine;
    const basic_sudoku_board_traits<S> traits;

    basic_sudoku_tile_patch<S> patch(w, h);
    basic_sudoku_tiling<S> bst{
      traits, traits.make_generator().generate_medium()};
    std::cout << bst.fill(0, 0, patch);
}

auto main(int argc, const char** argv) -> int {
    eagine::program_args args{argc, argv};

    sudoku_tiling<4>(
      args.find_value_or("--width", 32, std::cerr),
      args.find_value_or("--height", 24, std::cerr));

    return 0;
}
