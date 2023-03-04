/// @example eagine/integer_hash.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
import eagine.core;
import std;

namespace eagine {

template <typename T, typename V>
void print(const console& cio, V value) {
    cio.print(identifier{"hash"}, "hash(${value}) = ${hash}")
      .arg(identifier{"value"}, value)
      .arg(identifier{"hash"}, integer_hash<T>(value));
}

auto main(main_ctx& ctx) -> int {
    using namespace eagine;

    auto& cio = ctx.cio();

    print<std::uint16_t>(cio, 0);
    print<std::uint32_t>(cio, 1);
    print<std::uint64_t>(cio, 2);

    print<std::uint16_t>(cio, sizeof(short));
    print<std::uint32_t>(cio, sizeof(int));
    print<std::uint64_t>(cio, sizeof(long));

    return 0;
}

} // namespace eagine

auto main(int argc, const char** argv) -> int {
    return eagine::default_main(argc, argv, eagine::main);
}

