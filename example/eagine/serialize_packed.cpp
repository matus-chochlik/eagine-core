/// @example eagine/serialize_packed.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
import eagine.core;
import std;

namespace eagine {
//------------------------------------------------------------------------------
struct my_struct {
    bool b{false};
    char c{'\0'};
    double d{0.0};
    identifier i{};
    long l{};
    std::string s{};
    unsigned u{0U};
    std::string z{};
};
//------------------------------------------------------------------------------
template <>
struct data_member_traits<my_struct> {
    static constexpr auto mapping() noexcept {
        using S = my_struct;
        return make_data_member_mapping<
          S,
          bool,
          char,
          double,
          identifier,
          long,
          std::string,
          unsigned,
          std::string>(
          {"b", &S::b},
          {"c", &S::c},
          {"d", &S::d},
          {"i", &S::i},
          {"l", &S::l},
          {"s", &S::s},
          {"u", &S::u},
          {"z", &S::z});
    }
};
//------------------------------------------------------------------------------
void baz(const my_struct& instance) {
    std::cout << instance.b;
    std::cout << instance.c;
    std::cout << instance.d;
    std::cout << instance.i.name();
    std::cout << instance.l;
    std::cout << instance.s;
    std::cout << instance.u;
    std::cout << instance.z;
    std::cout << std::endl;
}
//------------------------------------------------------------------------------
void bar(main_ctx& ctx, const memory::const_block data) {
    packed_block_data_source source(data_compressor{ctx.buffers()}, data);
    std::cout << hexdump(source.remaining());
    string_deserializer_backend backend(source);
    my_struct instance;
    auto member_map = map_data_members(instance);
    if(deserialize(member_map, backend)) {
        baz(instance);
    }
}
//------------------------------------------------------------------------------
void foo(main_ctx& ctx, const my_struct& instance) {

    std::array<byte, 1024> data{};
    packed_block_data_sink sink(data_compressor{ctx.buffers()}, cover(data));
    string_serializer_backend backend(sink);
    auto member_map = map_data_members(instance);
    if(serialize(member_map, backend)) {
        std::cout << hexdump(sink.done());
        bar(ctx, sink.done());
    }
}
//------------------------------------------------------------------------------
auto main(main_ctx& ctx) -> int {
    using namespace eagine;

    my_struct x;
    x.b = true;
    x.c = '2';
    x.d = 3.4;
    x.i = "FiveSix";
    x.l = 7777777L;
    x.s = "eight times eight is sixty-four";
    x.u = 90U;
    x.z =
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod "
      "tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim "
      "veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea "
      "commodo consequat. Duis aute irure dolor in reprehenderit in voluptate "
      "velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint "
      "occaecat cupidatat non proident, sunt in culpa qui officia deserunt "
      "mollit anim id est laborum.";

    foo(ctx, x);

    return 0;
}
} // namespace eagine

auto main(int argc, const char** argv) -> int {
    return eagine::default_main(argc, argv, eagine::main);
}

