/// @example eagine/valid_if.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/valid_if/filesystem.hpp>
#include <eagine/valid_if/positive.hpp>
#include <iostream>

auto main(int argc, const char** argv) -> int {
    int i = 0;
    eagine::valid_if_positive<int&> vi{i};
    if(has_value(vi)) {
        std::cout << "A" << std::endl;
    }
    i = argc;
    if(has_value(vi)) {
        std::cout << "B" << std::endl;
    }
    vi = 0;
    if(has_value(vi)) {
        std::cout << "C|" << i << std::endl;
    }

    eagine::valid_if_existing_file<eagine::basic_string_path> vf{
      eagine::string_view{argv[0]}};
    if(has_value(vf)) {
        std::cout << "D|" << extract(vf) << std::endl;
    }

    eagine::valid_if_in_writable_directory<eagine::basic_string_path> vd{
      eagine::string_view{argv[0]}};
    if(has_value(vd)) {
        std::cout << "E" << std::endl;
    }

    return 0;
}

