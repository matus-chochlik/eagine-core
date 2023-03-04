/// @example eagine/embed_packed.cpp
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
    memory::buffer_pool buffers;
    data_compressor compressor{buffers};
    auto print = [](const memory::const_block data) {
        std::cout << hexdump(data);
        return true;
    };

    if(const auto res{search_resource("packedSelf")}) {
        std::cout << print(res.embedded_block()) << std::endl;
        std::cout << "format: " << enumerator_name(res.format()) << std::endl;
        std::cout << "is packed: " << res.is_packed() << std::endl;
        std::cout << res.fetch(compressor, {construct_from, print});
        std::cout << std::endl;
        auto unpacker{res.make_unpacker(buffers, {construct_from, print}, 64)};
        do {
            unpacker.next();
        } while(unpacker.is_working());
    }

    return 0;
}

