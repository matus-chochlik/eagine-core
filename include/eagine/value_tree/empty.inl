/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include <eagine/main_ctx_object.hpp>
#include <eagine/value_tree/implementation.hpp>

namespace eagine::valtree {
//------------------------------------------------------------------------------
class empty_compound
  : public main_ctx_object
  , public compound_implementation<empty_compound> {
public:
    empty_compound(main_ctx_parent parent)
      : main_ctx_object{EAGINE_ID(EmptyCmpnd), parent} {}

    static auto make_shared(main_ctx_parent parent)
      -> std::shared_ptr<empty_compound> {
        return std::make_shared<empty_compound>(parent);
    }

    auto type_id() const noexcept -> identifier_t final {
        return EAGINE_ID_V(empty);
    }

    void add_ref(attribute_interface&) noexcept final {}

    void release(attribute_interface&) noexcept final {}

    auto structure() -> attribute_interface* final {
        return nullptr;
    }

    auto attribute_name(attribute_interface&) -> string_view final {
        return {};
    }

    auto canonical_type(attribute_interface&) -> value_type final {
        return value_type::byte_type;
    }

    auto is_link(attribute_interface&) -> bool final {
        return false;
    }

    auto nested_count(attribute_interface&) -> span_size_t final {
        return 0;
    }

    auto nested(attribute_interface&, span_size_t)
      -> attribute_interface* final {
        return nullptr;
    }

    auto nested(attribute_interface&, string_view)
      -> attribute_interface* final {
        return nullptr;
    }

    auto find(attribute_interface&, const basic_string_path&)
      -> attribute_interface* final {
        return nullptr;
    }

    auto find(
      attribute_interface&,
      const basic_string_path&,
      span<const string_view>) -> attribute_interface* final {
        return nullptr;
    }

    auto value_count(attribute_interface&) -> span_size_t final {
        return 0;
    }

    template <typename T>
    auto do_fetch_values(attribute_interface&, span_size_t, span<T>)
      -> span_size_t {
        return 0;
    }
};
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
auto empty(main_ctx_parent parent) -> compound {
    return compound::make<empty_compound>(parent);
}
//------------------------------------------------------------------------------
} // namespace eagine::valtree
