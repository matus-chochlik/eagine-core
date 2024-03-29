/// @example eagine/sudoku_solver.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
import eagine.core;
import std;

template <unsigned S>
void solve_sudoku(const eagine::program_args& args) {
    using namespace eagine;
    const default_sudoku_board_traits<S> traits;

    std::stack<basic_sudoku_board<S>> solutions;

    if(args.find("--gen-one")) {
        solutions.push(traits.make_generator().generate_one());
    } else if(args.find("--gen-few")) {
        solutions.push(traits.make_generator().generate_few());
    } else if(args.find("--gen-medium")) {
        solutions.push(traits.make_generator().generate_medium());
    } else if(args.find("--gen-many")) {
        solutions.push(traits.make_generator().generate_many());
    } else {
        solutions.push(traits.make_diagonal());
    }

    const auto print = [&](const auto& b) {
        traits.print(std::cout, b);
    };
    auto board = solutions.top();
    print(board);

    bool done = false;
    while(not solutions.empty() and not done) {
        print(board);
        board = solutions.top();
        solutions.pop();

        board.for_each_alternative(board.find_unsolved(), [&](auto candidate) {
            if(candidate.is_solved()) {
                print(board);
                done = true;
            } else {
                solutions.push(candidate);
            }
        });
    }
}

auto main(int argc, const char** argv) -> int {
    eagine::program_args args{argc, argv};

    if(args.find("--4")) {
        solve_sudoku<4>(args);
    } else if(args.find("--2")) {
        solve_sudoku<2>(args);
    } else {
        solve_sudoku<3>(args);
    }

    return 0;
}
