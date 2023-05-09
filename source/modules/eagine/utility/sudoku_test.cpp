/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import std;
import eagine.core.types;
import eagine.core.utility;
//------------------------------------------------------------------------------
auto do_solve_sudoku(auto& test, auto board, eagine::timeout max_time) -> bool {
    eagitest::track trck{test, 0, 2};
    using namespace eagine;

    std::stack<decltype(board)> solutions;

    solutions.push(board);

    bool done = false;
    while(not solutions.empty() and not done) {
        if(max_time.is_expired()) {
            return false;
        }
        board = solutions.top();
        solutions.pop();

        board.for_each_alternative(
          board.find_unsolved(), [&](const auto& candidate) {
              if(candidate.is_solved()) {
                  trck.checkpoint(1);
                  done = true;
              } else {
                  trck.checkpoint(2);
                  solutions.push(candidate);
              }
          });
    }
    return true;
}
//------------------------------------------------------------------------------
// diagonal
//------------------------------------------------------------------------------
template <unsigned S>
void sudoku_rank_S_diagonal(auto& test, eagine::timeout max_time) {
    const eagine::default_sudoku_board_traits<S> traits;
    if(not do_solve_sudoku(test, traits.make_diagonal(), max_time)) {
        test.fail("solution time expired");
    }
}
//------------------------------------------------------------------------------
void sudoku_rank_2_diagonal(auto& s) {
    eagitest::case_ test{s, 1, "rank 2 diagonal"};
    sudoku_rank_S_diagonal<2>(test, std::chrono::seconds{1});
}
//------------------------------------------------------------------------------
void sudoku_rank_3_diagonal(auto& s) {
    eagitest::case_ test{s, 2, "rank 3 diagonal"};
    sudoku_rank_S_diagonal<3>(test, std::chrono::seconds{3});
}
//------------------------------------------------------------------------------
void sudoku_rank_4_diagonal(auto& s) {
    eagitest::case_ test{s, 3, "rank 4 diagonal"};
    sudoku_rank_S_diagonal<4>(test, std::chrono::seconds{9});
}
//------------------------------------------------------------------------------
// one
//------------------------------------------------------------------------------
template <unsigned S>
void sudoku_rank_S_one(auto& test, eagine::timeout max_time) {
    const eagine::default_sudoku_board_traits<S> traits;
    for(unsigned trial = 0; trial < 20; ++trial) {
        if(do_solve_sudoku(
             test, traits.make_generator().generate_one(), max_time)) {
            return;
        }
    }
    test.fail("solution time expired");
}
//------------------------------------------------------------------------------
void sudoku_rank_2_one(auto& s) {
    eagitest::case_ test{s, 4, "rank 2 one"};
    sudoku_rank_S_one<2>(test, std::chrono::seconds{2});
}
//------------------------------------------------------------------------------
void sudoku_rank_3_one(auto& s) {
    eagitest::case_ test{s, 5, "rank 3 one"};
    sudoku_rank_S_one<3>(test, std::chrono::seconds{9});
}
//------------------------------------------------------------------------------
void sudoku_rank_4_one(auto& s) {
    eagitest::case_ test{s, 6, "rank 4 one"};
    sudoku_rank_S_one<4>(test, std::chrono::seconds{16});
}
//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "sudoku", 6};
    test.once(sudoku_rank_2_diagonal);
    test.once(sudoku_rank_3_diagonal);
    test.once(sudoku_rank_4_diagonal);
    test.once(sudoku_rank_2_one);
    test.once(sudoku_rank_3_one);
    test.once(sudoku_rank_4_one);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>

