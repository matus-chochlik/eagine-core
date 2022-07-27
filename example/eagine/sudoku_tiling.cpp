/// @example eagine/sudoku_tiling.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#if EAGINE_CORE_MODULE
import eagine.core;
import <iostream>;
import <stack>;
import <map>;
#else
#include <eagine/program_args.hpp>
#include <eagine/sudoku.hpp>
#include <iostream>
#include <map>
#include <stack>
#endif

template <unsigned S>
void sudoku_tiling() {
    using namespace eagine;
    const basic_sudoku_board_traits<S> traits;

    basic_sudoku_tile_patch<S> patch(32, 24);
    basic_sudoku_tiling<S> bst{
      traits, traits.make_generator().generate_medium()};
    std::cout << bst.fill(0, 0, patch);
}

auto main() -> int {
    sudoku_tiling<4>();

    return 0;
}
