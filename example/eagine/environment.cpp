/// @example eagine/environment.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
import eagine.core;
import std;

auto main() -> int {
    using namespace eagine;

    using v = string_view;

    for(const auto name : view(
          {v("USER"),
           v("HOME"),
           v("PATH"),
           v("LANGUAGE"),
           v("SHELL"),
           v("PAGER"),
           v("EDITOR"),
           v("NONE")})) {

        std::cout << name << ": "
                  << get_environment_variable(name).value_or("N/A")
                  << std::endl;
    }

    return 0;
}
