/// @example eagine/scope_exit.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
import eagine.core;
import <exception>;
import <iostream>;

auto main() -> int {
    using namespace eagine;
    using std::cout;

    auto x = finally([]() { cout << "done\n"; });
    auto c = finally([]() { cout << "should not see this\n"; });
    cout << "starting\n";
    try {
        [[maybe_unused]] auto y{finally([] { cout << "leaving try block\n"; })};
        cout << "entering try block\n";
        {
            [[maybe_unused]] auto z{
              finally([] { cout << "leaving inner block\n"; })};
            cout << "entering inner block\n";
            throw std::exception();
        }
    } catch(...) {
        [[maybe_unused]] auto e{
          finally([] { cout << "leaving catch block\n"; })};
        cout << "entering catch block\n";
    }
    c.cancel();

    return 0;
}
