/// @example eagine/serialize_basic.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include <eagine/reflect/data_members.hpp>
#include <eagine/serialize/fast_backend.hpp>
#include <eagine/serialize/istream_source.hpp>
#include <eagine/serialize/ostream_sink.hpp>
#include <eagine/serialize/read.hpp>
#include <eagine/serialize/write.hpp>
#include <iostream>
#include <sstream>

namespace eagine {
//------------------------------------------------------------------------------
struct my_struct {
    bool b{false};
    char c{'\0'};
    float f{0.F};
    int i{0};
    std::string s{};
    unsigned u{0U};
};
//------------------------------------------------------------------------------
template <identifier_t Id>
constexpr auto data_member_mapping(
  const type_identity<my_struct>,
  const selector<Id>) noexcept {
    using S = my_struct;
    return make_data_member_mapping<S, bool, char, float, int, std::string, unsigned>(
      {"b", &S::b},
      {"c", &S::c},
      {"f", &S::f},
      {"i", &S::i},
      {"s", &S::s},
      {"u", &S::u});
}
//------------------------------------------------------------------------------
void baz(const my_struct& instance) {
    std::cout << instance.b;
    std::cout << instance.c;
    std::cout << instance.f;
    std::cout << instance.i;
    std::cout << instance.s;
    std::cout << instance.u;
    std::cout << std::endl;
}
//------------------------------------------------------------------------------
void bar(std::istream& data) {
    istream_data_source source(data);
    fast_deserializer_backend backend(source);
    my_struct instance;
    auto member_map = map_data_members(instance);
    deserialize(member_map, backend);
    baz(instance);
}
//------------------------------------------------------------------------------
void foo(const my_struct& instance) {

    std::stringstream data;
    ostream_data_sink sink(data);
    fast_serializer_backend backend(sink);
    auto member_map = map_data_members(instance);
    serialize(member_map, backend);
    bar(data);
}
//------------------------------------------------------------------------------
} // namespace eagine

auto main() -> int {
    using namespace eagine;

    my_struct x;
    x.b = true;
    x.c = '2';
    x.f = 34.5F;
    x.i = 67;
    x.s = "eight";
    x.u = 90U;

    foo(x);

    return 0;
}
