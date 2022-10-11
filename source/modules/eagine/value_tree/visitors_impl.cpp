/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

module eagine.core.value_tree;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.string;
import eagine.core.console;

namespace eagine::valtree {
//------------------------------------------------------------------------------
class combined_value_tree_visitor
  : public value_tree_visitor_impl<combined_value_tree_visitor> {
public:
    combined_value_tree_visitor(
      std::shared_ptr<value_tree_visitor> left,
      std::shared_ptr<value_tree_visitor> right) noexcept
      : _left{std::move(left)}
      , _right{std::move(right)} {}

    auto should_continue() noexcept -> bool {
        return _left->should_continue() && _right->should_continue();
    }

    void begin() noexcept final {
        _left->begin();
        _right->begin();
    }

    template <typename T>
    void do_consume(span<const T> data) noexcept {
        _left->consume(data);
        _right->consume(data);
    }

    void begin_struct() noexcept final {
        _left->begin_struct();
        _right->begin_struct();
    }

    void begin_attribute(const string_view name) noexcept final {
        _left->begin_attribute(name);
        _right->begin_attribute(name);
    }

    void finish_attribute(const string_view name) noexcept final {
        _left->finish_attribute(name);
        _right->finish_attribute(name);
    }

    void finish_struct() noexcept final {
        _left->finish_struct();
        _right->finish_struct();
    }

    void begin_list() noexcept final {
        _left->begin_list();
        _right->begin_list();
    }

    void finish_list() noexcept final {
        _left->finish_list();
        _right->finish_list();
    }

    void flush() noexcept final {
        _left->flush();
        _right->flush();
    }

    void unparsed_data(span<const memory::const_block> data) noexcept final {
        _left->unparsed_data(data);
        _right->unparsed_data(data);
    }

    void finish() noexcept final {
        _left->finish();
        _right->finish();
    }

    void failed() noexcept final {
        _left->failed();
        _right->finish();
    }

private:
    std::shared_ptr<value_tree_visitor> _left;
    std::shared_ptr<value_tree_visitor> _right;
};
//------------------------------------------------------------------------------
auto make_combined_value_tree_visitor(
  std::shared_ptr<value_tree_visitor> left,
  std::shared_ptr<value_tree_visitor> right)
  -> std::unique_ptr<value_tree_visitor> {
    return std::make_unique<combined_value_tree_visitor>(
      std::move(left), std::move(right));
}
//------------------------------------------------------------------------------
class printing_value_tree_visitor
  : public value_tree_visitor_impl<printing_value_tree_visitor> {
public:
    printing_value_tree_visitor(const console& cio)
      : _cio{cio.print("valTrePrnt", "printing value tree elements")
               .to_be_continued()} {}

    auto should_continue() noexcept -> bool {
        return true;
    }

    void begin() noexcept final {
        _cio.print("begin traversal");
    }

    void do_consume(span<const nothing_t> data) noexcept {
        _cio.print("consume nil: ${count] instances").arg("count", data.size());
    }

    template <typename T>
    void do_consume(span<const T> data) noexcept {
        const auto value_cio{_cio.print("consume:").to_be_continued()};

        for(const auto& value : data) {
            value_cio.print("'${value}'").arg("value", value);
        }
    }

    void begin_struct() noexcept final {
        _cio.print("enter structure");
    }

    void begin_attribute(const string_view name) noexcept final {
        _cio.print("enter attribute: ${name}").arg("name", name);
    }

    void finish_attribute(const string_view name) noexcept final {
        _cio.print("leave attribute: ${name}").arg("name", name);
    }

    void finish_struct() noexcept final {
        _cio.print("leave structure");
    }

    void begin_list() noexcept final {
        _cio.print("enter list");
    }

    void finish_list() noexcept final {
        _cio.print("leave list");
    }

    void flush() noexcept final {
        _cio.print("flush");
    }

    void unparsed_data(span<const memory::const_block>) noexcept final {
        _cio.print("unparsed data");
    }

    void finish() noexcept final {
        _cio.print("finish traversal");
    }

    void failed() noexcept final {
        _cio.print("traversal failed");
    }

private:
    console_entry_continuation _cio;
};
//------------------------------------------------------------------------------
auto make_printing_value_tree_visitor(const console& cio)
  -> std::unique_ptr<value_tree_visitor> {
    return std::make_unique<printing_value_tree_visitor>(cio);
}
//------------------------------------------------------------------------------
class building_value_tree_visitor
  : public value_tree_visitor_impl<building_value_tree_visitor> {
public:
    building_value_tree_visitor(std::shared_ptr<object_builder> builder) noexcept
      : _builder{std::move(builder)} {}

    auto should_continue() noexcept -> bool final {
        return _should_continue && _builder->should_continue();
    }

    void begin() noexcept final {
        _path.clear();
        _should_continue = true;
        _builder->begin();
    }

    template <typename T>
    void do_consume(span<const T> data) noexcept {
        _builder->add(_path, data);
    }

    void begin_struct() noexcept final {
        _builder->add_object(_path);
    }

    void begin_attribute(const string_view name) noexcept final {
        _path.push_back(name);
    }

    void finish_attribute(const string_view name) noexcept final {
        if(!_path.empty() && _path.back() == name) {
            _path.pop_back();
        } else {
            _should_continue = false;
        }
    }

    void finish_struct() noexcept final {
        _builder->finish_object(_path);
    }

    void begin_list() noexcept final {
        _path.push_back("_");
    }

    void finish_list() noexcept final {
        if(!_path.empty() && _path.back() == string_view{"_"}) {
            _path.pop_back();
        } else {
            _should_continue = false;
        }
    }

    void flush() noexcept final {}

    void unparsed_data(span<const memory::const_block> data) noexcept final {
        _builder->unparsed_data(data);
    }

    void finish() noexcept final {
        if(!_path.empty()) {
            _builder->failed();
        }
        _builder->finish();
    }

    void failed() noexcept final {
        _builder->failed();
    }

private:
    std::shared_ptr<object_builder> _builder;
    basic_string_path _path;
    bool _should_continue{false};
};
//------------------------------------------------------------------------------
auto make_building_value_tree_visitor(std::shared_ptr<object_builder> builder)
  -> std::unique_ptr<value_tree_visitor> {
    return std::make_unique<building_value_tree_visitor>(std::move(builder));
}
//------------------------------------------------------------------------------
} // namespace eagine::valtree
