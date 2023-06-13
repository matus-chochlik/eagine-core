/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <ryml.hpp>

module eagine.core.value_tree;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.string;
import eagine.core.identifier;
import eagine.core.valid_if;
import eagine.core.logging;

namespace eagine::valtree {
//------------------------------------------------------------------------------
static inline auto rapidyaml_cstrref(string_view str) noexcept {
    return c4::csubstr(str.data(), integer(str.size()));
}
//------------------------------------------------------------------------------
static inline auto view(c4::csubstr str) noexcept {
    return string_view{str.data(), integer(str.size())};
}
//------------------------------------------------------------------------------
class rapidyaml_callbacks {

    c4::yml::Callbacks _prev{};
    c4::yml::Callbacks _cbks{};

    [[noreturn]] void _do_handle_error(const std::string& msg) {
        throw std::runtime_error(msg);
    }

    [[noreturn]] static void _handle_error(
      const char* msg,
      size_t len,
      c4::yml::Location,
      void* self) {
        static_cast<rapidyaml_callbacks*>(self)->_do_handle_error(
          std::string(msg, len));
    }

public:
    rapidyaml_callbacks() {
#ifdef RYML_NO_DEFAULT_CALLBACKS
        _prev = {c4::yml::get_callbacks()};
        _cbks = {
          static_cast<void*>(this),
          _prev.m_allocate,
          _prev.m_free,
          &rapidyaml_callbacks::_handle_error};
#else
        _cbks = {
          static_cast<void*>(this),
          nullptr,
          nullptr,
          &rapidyaml_callbacks::_handle_error};
#endif
        c4::yml::set_callbacks(_cbks);
    }

    ~rapidyaml_callbacks() noexcept {
        c4::yml::set_callbacks(_prev);
    }

    rapidyaml_callbacks(rapidyaml_callbacks&&) = delete;
    rapidyaml_callbacks(const rapidyaml_callbacks&) = delete;
    auto operator=(rapidyaml_callbacks&&) = delete;
    auto operator=(const rapidyaml_callbacks&) = delete;
};
//------------------------------------------------------------------------------
class rapidyaml_tree_compound;
class rapidyaml_attribute;
static auto rapidyaml_make_new_node(
  rapidyaml_tree_compound&,
  ryml::ConstNodeRef) noexcept -> optional_reference<rapidyaml_attribute>;
//------------------------------------------------------------------------------
class rapidyaml_attribute : public attribute_interface {
    ryml::ConstNodeRef _node{};

    static inline auto _usable(const ryml::ConstNodeRef& n) noexcept {
        return n.valid();
    }

public:
    [[nodiscard]] rapidyaml_attribute(ryml::ConstNodeRef node)
      : _node{node} {}

    auto type_id() const noexcept -> identifier final {
        return "rapidyaml";
    }

    auto name() const noexcept -> string_view {
        if(_usable(_node) and _node.has_key()) {
            return view(_node.key());
        }
        return {};
    }

    auto preview() const noexcept -> optionally_valid<string_view> {
        if(_usable(_node) and _node.has_val()) {
            return view(_node.val());
        }
        return {};
    }

    auto canonical_type() const noexcept -> value_type {
        if(_usable(_node)) {
            if(_node.is_container()) {
                return value_type::composite;
            }
            return value_type::string_type;
        }
        return value_type::unknown;
    }

    auto nested_count() const noexcept -> span_size_t {
        if(_usable(_node)) {
            if(_node.is_container()) {
                return span_size(_node.num_children());
            }
        }
        return 0;
    }

    auto value_count() const noexcept -> span_size_t {
        if(_usable(_node)) {
            if(_node.is_seq()) {
                return span_size(_node.num_children());
            }
            if(not _node.is_map()) {
                return 1;
            }
        }
        return 0;
    }

    auto nested(rapidyaml_tree_compound& owner, span_size_t index) const noexcept
      -> optional_reference<attribute_interface> {
        if(_usable(_node)) {
            auto child{_node[integer(index)]};
            if(_usable(child)) {
                return rapidyaml_make_new_node(owner, child);
            }
        }
        return {};
    }

    auto nested(rapidyaml_tree_compound& owner, string_view name) const noexcept
      -> optional_reference<attribute_interface> {
        if(_usable(_node)) {
            if(_node.is_map()) {
                auto child{_node.find_child(rapidyaml_cstrref(name))};
                if(_usable(child)) {
                    return rapidyaml_make_new_node(owner, child);
                }
            }
        }
        return {};
    }

    auto find(
      rapidyaml_tree_compound& owner,
      const basic_string_path& path,
      span<const string_view> tags) const noexcept
      -> optional_reference<attribute_interface> {
        std::string temp_str;
        auto _cat =
          [&](string_view a, string_view b, string_view c) mutable -> auto& {
            return append_to(c, append_to(b, assign_to(a, temp_str)));
        };

        auto result{_node};
        for(auto& entry : path) {
            if(_usable(result)) {
                if(result.is_map()) {
                    bool found = false;
                    for(auto tag : tags) {
                        auto temp_node{result.find_child(
                          rapidyaml_cstrref(_cat(entry, "@", tag)))};
                        if(_usable(temp_node)) {
                            result = temp_node;
                            found = true;
                            break;
                        }
                    }
                    if(not found) {
                        result = result.find_child(rapidyaml_cstrref(entry));
                    }
                } else if(result.is_seq()) {
                    if(const auto opt_idx{from_string<span_size_t>(entry)}) {
                        result = result[*opt_idx];
                    } else {
                        result = ryml::ConstNodeRef{};
                    }
                } else {
                    result = ryml::ConstNodeRef{};
                }
            } else {
                break;
            }
        }

        if(_usable(result)) {
            return rapidyaml_make_new_node(owner, result);
        }
        return {};
    }

    auto fetch_values(span_size_t offset, span<char> dest) -> span_size_t {
        if(_usable(_node)) {
            if(not _node.is_seq()) {
                if(_node.has_val()) {
                    const auto src{head(skip(view(_node.val()), offset), dest)};
                    copy(src, dest);
                    return src.size();
                }
            }
        }
        return 0;
    }

    template <typename T>
    auto fetch_values(span_size_t offset, span<T> dest) -> span_size_t {
        if(_usable(_node)) {
            if(_node.is_seq()) {
                span_size_t pos{0};
                for(auto child : _node.children()) {
                    if(pos >= dest.size()) {
                        break;
                    }
                    if(child.has_val()) {
                        if(offset <= 0) {
                            if(auto opt_val{
                                 from_string<T>(view(child.val()))}) {
                                dest[pos] = std::move(*opt_val);
                                ++pos;
                            }
                        } else {
                            --offset;
                        }
                    }
                }
                return pos;
            }
            if(not dest.empty()) {
                if(_node.has_val()) {
                    if(auto opt_val{from_string<T>(view(_node.val()))}) {
                        dest.front() = std::move(*opt_val);
                        return 1;
                    }
                }
            }
        }
        return 0;
    }

    auto operator==(const rapidyaml_attribute& that) const noexcept {
        return _node == that._node;
    }
};
//------------------------------------------------------------------------------
class rapidyaml_tree_compound final
  : public compound_with_refcounted_node<
      rapidyaml_tree_compound,
      rapidyaml_attribute> {

    using base =
      compound_with_refcounted_node<rapidyaml_tree_compound, rapidyaml_attribute>;
    using base::_unwrap;

    logger _log;
    ryml::Tree _tree{};
    rapidyaml_attribute _root;

public:
    [[nodiscard]] rapidyaml_tree_compound(ryml::Tree tree, const logger& parent)
      : _log{"YamlValTre", parent}
      , _tree{std::move(tree)}
      , _root{_tree} {}

    rapidyaml_tree_compound(rapidyaml_tree_compound&&) = delete;
    rapidyaml_tree_compound(const rapidyaml_tree_compound&) = delete;
    auto operator=(rapidyaml_tree_compound&&) = delete;
    auto operator=(const rapidyaml_tree_compound&) = delete;

    ~rapidyaml_tree_compound() noexcept final = default;

    static auto make_shared(string_view yaml_text, const logger& parent)
      -> shared_holder<rapidyaml_tree_compound> {
        try {
            rapidyaml_callbacks cbks{};
            c4::csubstr src{yaml_text.data(), integer(yaml_text.size())};
            auto tree{ryml::parse_in_arena(src)};
            tree.resolve();
            return {default_selector, std::move(tree), parent};
        } catch(const std::runtime_error& err) {
            parent.log_error("YAML parse error: ${message}")
              .arg("message", string_view(err.what()));
        }
        return {};
    }

    auto type_id() const noexcept -> identifier final {
        return "rapidyaml";
    }

    auto structure() -> optional_reference<attribute_interface> final {
        return _root;
    }

    auto attribute_name(attribute_interface& attrib) -> string_view final {
        return _unwrap(attrib).name();
    }

    auto attribute_preview(attribute_interface& attrib)
      -> optionally_valid<string_view> final {
        return _unwrap(attrib).preview();
    }

    auto canonical_type(attribute_interface& attrib) -> value_type final {
        return _unwrap(attrib).canonical_type();
    }

    auto is_immutable(attribute_interface&) -> bool final {
        return true;
    }

    auto is_link(attribute_interface&) -> bool final {
        return false;
    }

    auto nested_count(attribute_interface& attrib) -> span_size_t final {
        return _unwrap(attrib).nested_count();
    }

    auto nested(attribute_interface& attrib, span_size_t index)
      -> optional_reference<attribute_interface> final {
        return _unwrap(attrib).nested(*this, index);
    }

    auto nested(attribute_interface& attrib, string_view name)
      -> optional_reference<attribute_interface> final {
        return _unwrap(attrib).nested(*this, name);
    }

    auto find(
      attribute_interface& attrib,
      const basic_string_path& path,
      span<const string_view> tags)
      -> optional_reference<attribute_interface> final {
        return _unwrap(attrib).find(*this, path, tags);
    }

    auto value_count(attribute_interface& attrib) -> span_size_t final {
        return _unwrap(attrib).value_count();
    }

    template <typename T>
    auto do_fetch_values(
      attribute_interface& attrib,
      span_size_t offset,
      span<T> dest) -> span_size_t {
        return _unwrap(attrib).fetch_values(offset, dest);
    }
};
//------------------------------------------------------------------------------
[[nodiscard]] static auto rapidyaml_make_new_node(
  rapidyaml_tree_compound& owner,
  ryml::ConstNodeRef node) noexcept -> optional_reference<rapidyaml_attribute> {
    return owner.make_node(node);
}
//------------------------------------------------------------------------------
[[nodiscard]] auto from_yaml_text(string_view yaml_text, const logger& parent)
  -> compound {
    return compound::make<rapidyaml_tree_compound>(yaml_text, parent);
}
//------------------------------------------------------------------------------
} // namespace eagine::valtree
