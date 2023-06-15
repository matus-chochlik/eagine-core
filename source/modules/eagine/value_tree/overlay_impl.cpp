/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
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
class overlay_compound;
class overlay_attribute;
static auto overlay_make_new_node(overlay_compound&, basic_string_path) noexcept
  -> optional_reference<overlay_attribute>;
//------------------------------------------------------------------------------
struct overlay_context {
    std::vector<compound_attribute> overlays;

    overlay_context(std::vector<compound_attribute> o) noexcept
      : overlays{std::move(o)} {}
};
//------------------------------------------------------------------------------
auto get_context(overlay_compound&) noexcept -> overlay_context&;
//------------------------------------------------------------------------------
class overlay_attribute : public attribute_interface {
    struct _child_info {
        std::size_t index{0};
        optional_reference<overlay_attribute> cached{};
    };

    std::map<std::string, _child_info, str_view_less> _children;
    basic_string_path _path{};
    bool _needs_scan{true};

    auto _scan(overlay_compound& owner) -> decltype(_children)& {
        if(_needs_scan) {
            bool all_immutable = true;
            const auto& overlays = get_context(owner).overlays;
            for(const auto index : integer_range(overlays.size())) {
                const auto& overlay = overlays[index];
                all_immutable &= overlay.is_immutable();
                if(const auto found{overlay.find(_path)}) {
                    for(const auto c : integer_range(found.nested_count())) {
                        auto name{to_string(found.nested(c).name())};
                        if(name.empty()) {
                            name = std::to_string(c);
                        }
                        auto pos = _children.find(name);
                        if(pos == _children.end()) {
                            pos = _children.try_emplace(std::move(name)).first;
                            pos->second.index = index;
                        }
                    }
                }
            }
            if(all_immutable) {
                _needs_scan = false;
            }
        }
        return _children;
    }

    auto _find_source(overlay_compound& owner) const -> compound_attribute {
        for(const auto& overlay : get_context(owner).overlays) {
            if(auto attrib{overlay.find(_path)}) {
                return attrib;
            }
        }
        return {};
    }

    auto _get_child(
      overlay_compound& owner,
      std::pair<const std::string, _child_info>& entry)
      -> optional_reference<overlay_attribute> {
        auto& info = entry.second;
        if(not info.cached) {
            basic_string_path path{_path};
            path.push_back(entry.first);
            info.cached = overlay_make_new_node(owner, std::move(path));
        }
        return info.cached;
    }

public:
    [[nodiscard]] overlay_attribute(basic_string_path path) noexcept
      : _path{std::move(path)} {}

    auto type_id() const noexcept -> identifier final {
        return "overlay";
    }

    friend auto operator==(
      const overlay_attribute& l,
      const overlay_attribute& r) noexcept -> bool {
        return l._path == r._path;
    }

    void mark_for_scan() {
        _needs_scan = true;
        for(auto& entry : _children) {
            if(auto child{entry.second.cached}) {
                child->mark_for_scan();
            }
        }
    }

    auto name() -> string_view {
        if(_path.empty()) [[unlikely]] {
            return {};
        }
        return _path.back();
    }

    auto preview(overlay_compound& owner) -> optionally_valid<string_view> {
        return _find_source(owner).preview();
    }

    auto canonical_type(overlay_compound& owner) -> value_type {
        return _find_source(owner).canonical_type();
    }

    auto is_immutable(overlay_compound& owner) const -> bool {
        return _find_source(owner).is_immutable();
    }

    auto is_link(overlay_compound& owner) const -> bool {
        return _find_source(owner).is_link();
    }

    auto nested_count(overlay_compound& owner) -> span_size_t {
        return span_size(_scan(owner).size());
    }

    auto nested(overlay_compound& owner, span_size_t index)
      -> optional_reference<attribute_interface> {
        auto& children = _scan(owner);
        if(index < span_size(children.size())) {
            auto pos = children.begin();
            std::advance(pos, index);
            return _get_child(owner, *pos);
        }
        return {};
    }

    auto nested(overlay_compound& owner, string_view name)
      -> optional_reference<attribute_interface> {
        auto& children = _scan(owner);
        auto pos = children.find(name);
        if(pos != children.end()) {
            return _get_child(owner, *pos);
        }
        return {};
    }

    auto find(
      [[maybe_unused]] overlay_compound& owner,
      [[maybe_unused]] const basic_string_path& path,
      [[maybe_unused]] span<const string_view> tags)
      -> optional_reference<attribute_interface> {
        return {};
    }

    auto value_count(overlay_compound& owner) -> span_size_t {
        return _find_source(owner).value_count();
    }

    template <typename T>
    auto fetch_values(overlay_compound& owner, span_size_t offset, span<T> dest)
      -> span_size_t {
        return _find_source(owner).fetch_values(offset, dest).size();
    }
};
//------------------------------------------------------------------------------
class overlay_compound
  : public compound_with_refcounted_node<overlay_compound, overlay_attribute> {
    using base =
      compound_with_refcounted_node<overlay_compound, overlay_attribute>;
    using base::_unwrap;

    logger _log;
    overlay_context _context;
    overlay_attribute _root{{}};

public:
    [[nodiscard]] overlay_compound(
      const logger& parent,
      std::vector<compound_attribute> overlays)
      : _log{"OvrlyCmpnd", parent}
      , _context{std::move(overlays)} {}

    static auto make_shared(
      const logger& parent,
      std::vector<compound_attribute> overlays)
      -> shared_holder<overlay_compound> {
        return {default_selector, parent, std::move(overlays)};
    }

    auto type_id() const noexcept -> identifier final {
        return "overlay";
    }

    auto structure() -> optional_reference<attribute_interface> final {
        return _root;
    }

    auto attribute_name(attribute_interface& attrib) -> string_view final {
        return _unwrap(attrib).name();
    }

    auto attribute_preview(attribute_interface& attrib)
      -> optionally_valid<string_view> final {
        return _unwrap(attrib).preview(*this);
    }

    auto canonical_type(attribute_interface& attrib) -> value_type final {
        return _unwrap(attrib).canonical_type(*this);
    }

    auto is_immutable(attribute_interface& attrib) -> bool final {
        return _unwrap(attrib).is_immutable(*this);
    }

    auto is_link(attribute_interface& attrib) -> bool final {
        return _unwrap(attrib).is_link(*this);
    }

    auto nested_count(attribute_interface& attrib) -> span_size_t final {
        return _unwrap(attrib).nested_count(*this);
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
        return _unwrap(attrib).value_count(*this);
    }

    template <typename T>
    auto do_fetch_values(
      attribute_interface& attrib,
      span_size_t offset,
      span<T> dest) -> span_size_t {
        return _unwrap(attrib).fetch_values(*this, offset, dest);
    }

    void add_overlay(compound_attribute overlay) {
        _root.mark_for_scan();
        _context.overlays.insert(_context.overlays.begin(), std::move(overlay));
    }

    friend auto get_context(overlay_compound& that) noexcept
      -> overlay_context& {
        return that._context;
    }
};
//------------------------------------------------------------------------------
static auto overlay_make_new_node(
  overlay_compound& owner,
  basic_string_path path) noexcept -> optional_reference<overlay_attribute> {
    return owner.make_node(std::move(path));
}
//------------------------------------------------------------------------------
[[nodiscard]] auto make_overlay(
  const logger& parent,
  std::vector<compound_attribute> overlays) -> compound {
    return compound::make<overlay_compound>(parent, std::move(overlays));
}
//------------------------------------------------------------------------------
auto add_overlay(compound& c, const compound_attribute& overlay) -> bool {
    if(auto oc{c.as<overlay_compound>()}) {
        oc->add_overlay(std::move(overlay));
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------
} // namespace eagine::valtree
