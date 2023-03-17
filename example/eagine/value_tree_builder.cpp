/// @example eagine/valtree_builder.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
import eagine.core;
import std;

namespace eagine {
//------------------------------------------------------------------------------
struct point {
    float x{}, y{}, z{};
};

template <identifier_t Id>
constexpr auto data_member_mapping(
  const std::type_identity<point>,
  const selector<Id>) noexcept {
    return make_data_member_mapping<point, float, float, float>(
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
class example_builder
  : public valtree::object_builder_impl<example_builder<O>> {

public:
    example_builder(O& obj) noexcept
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
auto make_builder(O& obj) {
    return std::make_unique<example_builder<O>>(obj);
}
//------------------------------------------------------------------------------
void build_and_traverse(main_ctx& ctx, const embedded_resource& res) {
    if(res) {
        tetrahedron t{};
        res.build(ctx, make_builder(t));
        traverse_mapped(t, [&](const auto& path, const auto& value) {
            ctx.cio()
              .print("traverse", "${path}: ${value}")
              .arg("path", path)
              .arg("value", value);
        });
    }
}
//------------------------------------------------------------------------------
auto main(main_ctx& ctx) -> int {

    build_and_traverse(ctx, search_resource("json1"));
    build_and_traverse(ctx, search_resource("json2"));

    return 0;
}
//------------------------------------------------------------------------------
} // namespace eagine

auto main(int argc, const char** argv) -> int {
    return eagine::default_main(argc, argv, eagine::main);
}

