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
import eagine.core.string;
import eagine.core.utility;
import eagine.core.runtime;
import eagine.core.logging;
import :interface;
import :wrappers;
import <chrono>;
import <memory>;
import <vector>;
import <type_traits>;

namespace eagine::valtree {
//------------------------------------------------------------------------------
/// @brief Class handling value tree stream input.
/// @ingroup valtree
export class value_tree_stream_input {
public:
    value_tree_stream_input(
      std::unique_ptr<value_tree_stream_parser> impl) noexcept
      : _pimpl{std::move(impl)} {
        _pimpl->begin();
    }

    value_tree_stream_input(value_tree_stream_input&&) noexcept = default;
    value_tree_stream_input(const value_tree_stream_input&) = delete;
    auto operator=(value_tree_stream_input&&) noexcept
      -> value_tree_stream_input& = default;
    auto operator=(const value_tree_stream_input&) = delete;

    ~value_tree_stream_input() noexcept {
        if(_pimpl) {
            _pimpl->finish();
        }
    }

    /// @brief Indicates if this stream input object is in a valid state.
    /// @see consume_data
    explicit operator bool() const noexcept {
        return bool(_pimpl);
    }

    /// @brief Consumes the next chunk of stream binary data.
    /// @see consume_text
    /// @see finish
    ///
    /// If the return value is true then the next chunk can be consumed,
    /// otherwise no further data should be provided and finish should be called.
    auto consume_data(const memory::const_block data) noexcept -> bool {
        assert(_pimpl);
        return _pimpl->parse_data(data);
    }

    /// @brief Consumes the next chunk of stream text data.
    /// @see consume_data
    /// @see finish
    ///
    /// If the return value is true then the next chunk can be consumed,
    /// otherwise no further data should be provided and finish should be called.
    auto consume_text(const memory::string_view data) noexcept -> bool {
        assert(_pimpl);
        return _pimpl->parse_data(memory::as_bytes(data));
    }

    /// @brief Finishes the consumption on input data.
    /// @see consume_data
    /// @see consume_text
    auto finish() noexcept -> bool {
        if(_pimpl) {
            const bool result{_pimpl->finish()};
            _pimpl.reset();
            return result;
        }
        return false;
    }

    /// @brief Alias for the data handler callable type.
    using data_handler = callable_ref<bool(memory::const_block) noexcept>;

    /// @brief Returns the callable that can consume stream data.
    /// @see consume_data
    auto get_handler() noexcept -> data_handler {
        assert(_pimpl);
        return make_callable_ref<&value_tree_stream_parser::parse_data>(
          _pimpl.get());
    }

private:
    std::unique_ptr<value_tree_stream_parser> _pimpl;
};
//------------------------------------------------------------------------------
export template <typename Derived>
struct value_tree_visitor_impl : value_tree_visitor {
    auto derived() noexcept -> Derived& {
        return *static_cast<Derived*>(this);
    }

    void consume(span<const nothing_t> data) noexcept final {
        derived().do_consume(data);
    }
    void consume(span<const bool> data) noexcept final {
        derived().do_consume(data);
    }
    void consume(span<const std::int64_t> data) noexcept final {
        derived().do_consume(data);
    }
    void consume(span<const std::uint64_t> data) noexcept final {
        derived().do_consume(data);
    }
    void consume(span<const float> data) noexcept final {
        derived().do_consume(data);
    }
    void consume(span<const double> data) noexcept final {
        derived().do_consume(data);
    }
    void consume(span<const string_view> data) noexcept final {
        derived().do_consume(data);
    }
};
//------------------------------------------------------------------------------
export template <typename Derived>
struct object_builder_impl : object_builder {
    auto derived() noexcept -> Derived& {
        return *static_cast<Derived*>(this);
    }

    void add(
      const basic_string_path& path,
      const span<const nothing_t> data) noexcept final {
        derived().do_add(path, data);
    }
    void add(const basic_string_path& path, span<const bool> data) noexcept
      final {
        derived().do_add(path, data);
    }
    void add(
      const basic_string_path& path,
      span<const std::int64_t> data) noexcept final {
        derived().do_add(path, data);
    }
    void add(
      const basic_string_path& path,
      span<const std::uint64_t> data) noexcept final {
        derived().do_add(path, data);
    }
    void add(const basic_string_path& path, span<const float> data) noexcept
      final {
        derived().do_add(path, data);
    }
    void add(const basic_string_path& path, span<const double> data) noexcept
      final {
        derived().do_add(path, data);
    }
    void add(
      const basic_string_path& path,
      span<const string_view> data) noexcept final {
        derived().do_add(path, data);
    }
};
//------------------------------------------------------------------------------
export class object_builder_data_forwarder {
    template <typename T>
    struct _fwd_func {
        span<const T> data;

        template <assignable_if_fits_from<T> X>
        auto operator()(X& dest) const noexcept -> bool {
            return assign_if_fits(data, dest);
        }

        template <typename X>
            requires(assignable_if_fits_from<X, T>)
        auto operator()(std::optional<X>& dest) const noexcept -> bool {
            return assign_if_fits(data, dest);
        }

        template <typename X>
            requires(not assignable_if_fits_from<X, T>)
        auto operator()(const X&) const noexcept -> bool {
            return false;
        }
    };

public:
    template <typename T, typename O>
    auto forward_data(
      const basic_string_path& path,
      span<const T> data,
      O& dest) {
        visit_mapped(_temp, path, dest, default_selector, _fwd_func<T>{data});
    }

private:
    basic_string_path _temp;
};
//------------------------------------------------------------------------------
export auto traverse_json_stream(
  std::shared_ptr<value_tree_visitor>,
  span_size_t max_token_size,
  memory::buffer_pool&,
  const logger& parent) -> value_tree_stream_input;

export auto traverse_json_stream(
  std::shared_ptr<object_builder>,
  memory::buffer_pool&,
  const logger& parent) -> value_tree_stream_input;
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
/// @brief Creates a compound from a collection of consecutive JSON data block
/// @ingroup valtree
export auto from_json_data(span<const memory::const_block>, const logger&)
  -> compound;
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
