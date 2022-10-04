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
import eagine.core.console;

namespace eagine::valtree {
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

    void begin() final {
        _cio.print("begin traversal");
    }

    void do_consume(span<const nothing_t> data) {
        _cio.print("consume nil: ${count] instances").arg("count", data.size());
    }

    template <typename T>
    void do_consume(span<const T> data) {
        const auto value_cio{_cio.print("consume:").to_be_continued()};

        for(const auto& value : data) {
            value_cio.print("'${value}'").arg("value", value);
        }
    }

    void begin_struct() final {
        _cio.print("enter structure");
    }

    void begin_attribute(const string_view name) final {
        _cio.print("enter attribute: ${name}").arg("name", name);
    }

    void finish_attribute(const string_view name) final {
        _cio.print("leave attribute: ${name}").arg("name", name);
    }

    void finish_struct() final {
        _cio.print("leave structure");
    }

    void begin_list() final {
        _cio.print("enter list");
    }

    void finish_list() final {
        _cio.print("leave list");
    }

    void flush() final {
        _cio.print("flush");
    }

    void finish() final {
        _cio.print("finish traversal");
    }

    void failed() final {
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
} // namespace eagine::valtree
