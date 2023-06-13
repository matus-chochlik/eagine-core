/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <eagine/testing/unit_begin.hpp>
import eagine.core.types;
import eagine.core.memory;
import eagine.core.string;
import eagine.core.reflection;
import eagine.core.logging;
import eagine.core.value_tree;
//------------------------------------------------------------------------------
namespace eagine {
//------------------------------------------------------------------------------
struct point {
    int x{}, y{}, z{};
};

template <identifier_t Id>
constexpr auto data_member_mapping(
  const std::type_identity<point>,
  const selector<Id>) noexcept {
    return make_data_member_mapping<point, int, int, int>(
      {"x", &point::x}, {"y", &point::y}, {"z", &point::z});
}
//------------------------------------------------------------------------------
struct triangle {
    point a{}, b{}, c{};
};

template <identifier_t Id>
constexpr auto data_member_mapping(
  const std::type_identity<triangle>,
  const selector<Id>) noexcept {
    return make_data_member_mapping<triangle, point, point, point>(
      {"a", &triangle::a}, {"b", &triangle::b}, {"c", &triangle::c});
}
//------------------------------------------------------------------------------
struct tetrahedron {
    triangle base{};
    point apex{};
};

template <identifier_t Id>
constexpr auto data_member_mapping(
  const std::type_identity<tetrahedron>,
  const selector<Id>) noexcept {
    return make_data_member_mapping<tetrahedron, triangle, point>(
      {"base", &tetrahedron::base}, {"apex", &tetrahedron::apex});
}
//------------------------------------------------------------------------------
template <typename O>
class test_builder : public valtree::object_builder_impl<test_builder<O>> {

public:
    test_builder(O& obj) noexcept
      : _obj{obj} {}

    auto max_token_size() noexcept -> span_size_t final {
        return 16;
    }

    template <typename T>
    void do_add(const basic_string_path& path, span<const T> data) noexcept {
        _forwarder.forward_data(path, data, _obj);
    }

    void failed() noexcept final {}

private:
    O& _obj;
    valtree::object_builder_data_forwarder _forwarder;
};
template <typename O>
auto make_builder(O& obj) -> unique_holder<test_builder<O>> {
    return {default_selector, obj};
}
//------------------------------------------------------------------------------
} // namespace eagine
//------------------------------------------------------------------------------
void json_builder_th(
  auto& test,
  eagine::memory::string_view json_text,
  eagine::tetrahedron orig) {
    eagine::tetrahedron bilt{};
    auto builder{eagine::make_builder(bilt)};
    test.ensure(bool(builder), "builder ok");
    const auto max_token_size{builder->max_token_size()};

    test.check(max_token_size > 0, "max token size");

    eagine::memory::buffer_pool buffers;
    eagine::logger log;

    auto input{eagine::valtree::traverse_json_stream(
      eagine::valtree::make_building_value_tree_visitor(std::move(builder)),
      max_token_size,
      buffers,
      log)};

    test.check(input.consume_text(json_text), "consume ok");
    test.check(input.finish(), "finish ok");

    test.check_equal(orig.base.a.x, bilt.base.a.x, "base.a.x");
    test.check_equal(orig.base.a.y, bilt.base.a.y, "base.a.y");
    test.check_equal(orig.base.a.z, bilt.base.a.z, "base.a.z");
    test.check_equal(orig.base.b.x, bilt.base.b.x, "base.b.x");
    test.check_equal(orig.base.b.y, bilt.base.b.y, "base.b.y");
    test.check_equal(orig.base.b.z, bilt.base.b.z, "base.b.z");
    test.check_equal(orig.base.c.x, bilt.base.c.x, "base.c.x");
    test.check_equal(orig.base.c.y, bilt.base.c.y, "base.c.y");
    test.check_equal(orig.base.c.z, bilt.base.c.z, "base.c.z");
    test.check_equal(orig.apex.x, bilt.apex.x, "apex.x");
    test.check_equal(orig.apex.y, bilt.apex.y, "apex.y");
    test.check_equal(orig.apex.z, bilt.apex.z, "apex.z");
}
//------------------------------------------------------------------------------
void json_builder_1(auto& s) {
    eagitest::case_ test{s, 1, "JSON builder 1"};

    json_builder_th(
      test,
      R"({"base":{"a":{"x":1},"b":{"y":1},"c":{"z":1}},"apex":{"x":0,"y":0,"z":0}})",
      {{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}, {0, 0, 0}});
}
//------------------------------------------------------------------------------
void json_builder_2(auto& s) {
    eagitest::case_ test{s, 2, "JSON builder 2"};
    auto& rg{test.random()};

    for(unsigned r = 0; r < test.repeats(100); ++r) {

        const auto bax{rg.get_any<int>()};
        const auto bay{rg.get_any<int>()};
        const auto baz{rg.get_any<int>()};
        const auto bbx{rg.get_any<int>()};
        const auto bby{rg.get_any<int>()};
        const auto bbz{rg.get_any<int>()};
        const auto bcx{rg.get_any<int>()};
        const auto bcy{rg.get_any<int>()};
        const auto bcz{rg.get_any<int>()};
        const auto ax{rg.get_any<int>()};
        const auto ay{rg.get_any<int>()};
        const auto az{rg.get_any<int>()};

        std::string json_text{R"({"apex":{"x":)"};
        json_text.append(std::to_string(ax));
        json_text.append(R"(,"y":)");
        json_text.append(std::to_string(ay));
        json_text.append(R"(,"z":)");
        json_text.append(std::to_string(az));
        json_text.append(R"(},"base":{"a":{"x":)");
        json_text.append(std::to_string(bax));
        json_text.append(R"(,"y":)");
        json_text.append(std::to_string(bay));
        json_text.append(R"(,"z":)");
        json_text.append(std::to_string(baz));
        json_text.append(R"(},"b":{"x":)");
        json_text.append(std::to_string(bbx));
        json_text.append(R"(,"y":)");
        json_text.append(std::to_string(bby));
        json_text.append(R"(,"z":)");
        json_text.append(std::to_string(bbz));
        json_text.append(R"(},"c":{"x":)");
        json_text.append(std::to_string(bcx));
        json_text.append(R"(,"y":)");
        json_text.append(std::to_string(bcy));
        json_text.append(R"(,"z":)");
        json_text.append(std::to_string(bcz));
        json_text.append(R"(}}})");

        json_builder_th(
          test,
          json_text,
          {{{bax, bay, baz}, {bbx, bby, bbz}, {bcx, bcy, bcz}}, {ax, ay, az}});
    }
}
//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
auto main(int argc, const char** argv) -> int {
    eagitest::suite test{argc, argv, "value_tree wrappers", 2};
    test.once(json_builder_1);
    test.once(json_builder_2);
    return test.exit_code();
}
//------------------------------------------------------------------------------
#include <eagine/testing/unit_end.hpp>
