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
import eagine.core.identifier;
import eagine.core.utility;
import eagine.core.logging;
import <chrono>;
import <memory>;
import <optional>;
import <string>;
import <tuple>;
import <vector>;

namespace eagine::valtree {
//------------------------------------------------------------------------------
void compound::traverse(const stack_visit_handler visit) const {
    std::vector<attribute> atr;
    std::vector<span_size_t> pos;
    basic_string_path path{};

    atr.push_back(structure());
    pos.push_back(0);

    while(true) {
        assert(atr.size() == pos.size());

        if(pos.back() < nested_count(atr.back())) {
            auto child{nested(atr.back(), pos.back())};
            if(const auto n{attribute_name(child)}) {
                path.push_back(n);
            } else {
                path.push_back(std::to_string(pos.back()));
            }
            if(visit(*this, child, path, view(atr)) and not is_link(child)) {
                atr.push_back(child);
                pos.push_back(0);
                continue;
            }
        } else {
            atr.pop_back();
            pos.pop_back();
        }
        if(pos.empty()) {
            break;
        }
        ++pos.back();
        path.pop_back();
    }
}
//------------------------------------------------------------------------------
void compound::traverse(const visit_handler visit) const {
    const auto adapted = [visit](
                           const compound& c,
                           const attribute& a,
                           const basic_string_path& p,
                           span<const attribute>) -> bool {
        return visit(c, a, p);
    };
    traverse(stack_visit_handler{construct_from, adapted});
}
//------------------------------------------------------------------------------
} // namespace eagine::valtree
