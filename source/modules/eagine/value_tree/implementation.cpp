/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.value_tree:implementation;

import eagine.core.concepts;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.logging;
import :interface;
import :wrappers;
import <chrono>;
import <memory>;
import <vector>;

namespace eagine::valtree {
//------------------------------------------------------------------------------
template <typename Derived>
class compound_implementation : public compound_interface {
private:
    auto derived() noexcept -> Derived& {
        return *static_cast<Derived*>(this);
    }

public:
    auto fetch_values(
      attribute_interface& attrib,
      span_size_t offset,
      span<bool> dest) -> span_size_t final {
        return derived().do_fetch_values(attrib, offset, dest);
    }

    auto fetch_values(
      attribute_interface& attrib,
      span_size_t offset,
      span<tribool> dest) -> span_size_t final {
        return derived().do_fetch_values(attrib, offset, dest);
    }

    auto fetch_values(
      attribute_interface& attrib,
      span_size_t offset,
      span<char> dest) -> span_size_t final {
        return derived().do_fetch_values(attrib, offset, dest);
    }

    auto fetch_values(
      attribute_interface& attrib,
      span_size_t offset,
      span<byte> dest) -> span_size_t final {
        return derived().do_fetch_values(attrib, offset, dest);
    }

    auto fetch_values(
      attribute_interface& attrib,
      span_size_t offset,
      span<std::int16_t> dest) -> span_size_t final {
        return derived().do_fetch_values(attrib, offset, dest);
    }

    auto fetch_values(
      attribute_interface& attrib,
      span_size_t offset,
      span<std::int32_t> dest) -> span_size_t final {
        return derived().do_fetch_values(attrib, offset, dest);
    }

    auto fetch_values(
      attribute_interface& attrib,
      span_size_t offset,
      span<std::int64_t> dest) -> span_size_t final {
        return derived().do_fetch_values(attrib, offset, dest);
    }

    auto fetch_values(
      attribute_interface& attrib,
      span_size_t offset,
      span<std::uint16_t> dest) -> span_size_t final {
        return derived().do_fetch_values(attrib, offset, dest);
    }

    auto fetch_values(
      attribute_interface& attrib,
      span_size_t offset,
      span<std::uint32_t> dest) -> span_size_t final {
        return derived().do_fetch_values(attrib, offset, dest);
    }

    auto fetch_values(
      attribute_interface& attrib,
      span_size_t offset,
      span<std::uint64_t> dest) -> span_size_t final {
        return derived().do_fetch_values(attrib, offset, dest);
    }

    auto fetch_values(
      attribute_interface& attrib,
      span_size_t offset,
      span<float> dest) -> span_size_t final {
        return derived().do_fetch_values(attrib, offset, dest);
    }

    auto fetch_values(
      attribute_interface& attrib,
      span_size_t offset,
      span<std::chrono::duration<float>> dest) -> span_size_t final {
        return derived().do_fetch_values(attrib, offset, dest);
    }

    auto fetch_values(
      attribute_interface& attrib,
      span_size_t offset,
      span<std::string> dest) -> span_size_t final {
        return derived().do_fetch_values(attrib, offset, dest);
    }
};
//------------------------------------------------------------------------------
template <typename Derived, typename Node>
class compound_with_refcounted_node : public compound_implementation<Derived> {
private:
    std::vector<std::tuple<span_size_t, std::unique_ptr<Node>>> _nodes{};

    auto _do_make_new(Node&& temp) -> Node* {
        for(auto& [ref_count, node_ptr] : _nodes) {
            if(temp == *node_ptr) {
                ++ref_count;
                return node_ptr.get();
            }
        }
        _nodes.emplace_back(1, std::make_unique<Node>(std::move(temp)));
        return std::get<1>(_nodes.back()).get();
    }

protected:
    auto _unwrap(attribute_interface& attrib) const noexcept -> auto& {
        assert(attrib.type_id() == this->type_id());
        assert(dynamic_cast<Node*>(&attrib));
        return static_cast<Node&>(attrib);
    }

public:
    template <typename... Args>
    auto make_node(Args&&... args) {
        return _do_make_new(Node{std::forward<Args>(args)...});
    }

    void add_ref(attribute_interface& attrib) noexcept final {
        auto& that = _unwrap(attrib);
        for(auto& [ref_count, node_ptr] : _nodes) {
            if(&that == node_ptr.get()) {
                ++ref_count;
            }
        }
    }

    void release(attribute_interface& attrib) noexcept final {
        auto& that = _unwrap(attrib);
        for(auto pos = _nodes.begin(); pos != _nodes.end(); ++pos) {
            auto& [ref_count, node_ptr] = *pos;
            if(&that == node_ptr.get()) {
                if(--ref_count <= 0) {
                    _nodes.erase(pos);
                    break;
                }
            }
        }
    }
};
//------------------------------------------------------------------------------
/// @brief Creates a compound representing an empty subtree.
/// @ingroup valtree
/// @see from_filesystem_path
/// @see from_json_text
/// @see from_yaml_text
export auto empty(const logger&) -> compound;

export auto empty(does_not_hide<logger> auto& parent) -> compound {
    return empty(parent.log());
}
//------------------------------------------------------------------------------
export struct file_compound_factory : interface<file_compound_factory> {
    virtual auto make_compound(string_view path, const logger&) -> compound = 0;
};
//------------------------------------------------------------------------------
/// @brief Creates a compound representing a filesystem subtree.
/// @ingroup valtree
export auto from_filesystem_path(
  string_view root_path,
  const logger&,
  std::shared_ptr<file_compound_factory> = {}) -> compound;

export auto from_filesystem_path(
  string_view root_path,
  does_not_hide<logger> auto& parent,
  std::shared_ptr<file_compound_factory> factory = {}) -> compound {
    return from_filesystem_path(root_path, parent, std::move(factory));
}
//------------------------------------------------------------------------------
/// @brief Creates a compound from a JSON text string view.
/// @ingroup valtree
export auto from_json_text(string_view, const logger&) -> compound;

export auto from_json_text(
  string_view json_text,
  does_not_hide<logger> auto& parent) -> compound {
    return from_json_text(json_text, parent.log());
}
//------------------------------------------------------------------------------
/// @brief Creates a compound from a YAML text string view.
/// @ingroup valtree
export auto from_yaml_text(string_view, const logger&) -> compound;

export auto from_yaml_text(
  string_view yaml_text,
  does_not_hide<logger> auto& parent) -> compound {
    return from_yaml_text(yaml_text, parent.log());
}
//------------------------------------------------------------------------------
/// @brief Creates a overlay compound, combining multiple other compounds.
/// @ingroup valtree
export auto make_overlay(const logger& parent, std::vector<compound_attribute>)
  -> compound;

export auto make_overlay(
  does_not_hide<logger> auto& parent,
  std::vector<compound_attribute> attribs) -> compound {
    return make_overlay(parent.log(), attribs);
}

export auto make_overlay(const logger& parent) -> compound {
    return make_overlay(parent, {});
}

export auto make_overlay(does_not_hide<logger> auto& parent) -> compound {
    return make_overlay(parent, {});
}
//------------------------------------------------------------------------------
export auto add_overlay(compound&, const compound_attribute&) -> bool;

export auto add_overlay(compound& c, const compound& a) -> bool {
    return add_overlay(c, a.root());
}
//------------------------------------------------------------------------------
} // namespace eagine::valtree
