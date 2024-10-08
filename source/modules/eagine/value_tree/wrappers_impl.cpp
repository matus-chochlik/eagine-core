/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

module eagine.core.value_tree;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.string;
import eagine.core.identifier;
import eagine.core.utility;
import eagine.core.runtime;
import eagine.core.logging;

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
void compound_traverse_foward_bool(
  const shared_holder<value_tree_visitor>& visitor,
  const compound& cmpd,
  const attribute& attr,
  const span_size_t value_count) {
    if(value_count == 1) {
        bool value;
        if(cmpd.fetch_values(attr, cover_one(value))) {
            visitor->consume(view_one(value));
        }
    } else if(value_count <= 256) {
        std::array<bool, 256> values;
        if(cmpd.fetch_values(attr, head(cover(values), value_count))) {
            visitor->consume(head(view(values), value_count));
        }
    }
}
//------------------------------------------------------------------------------
void compound_traverse_foward_int(
  const shared_holder<value_tree_visitor>& visitor,
  const compound& cmpd,
  const attribute& attr,
  const span_size_t value_count) {
    if(value_count == 1) {
        std::int64_t value;
        if(cmpd.fetch_values(attr, cover_one(value))) {
            visitor->consume(view_one(value));
        }
    } else {
        std::vector<std::int64_t> values;
        values.resize(std_size(value_count));
        if(cmpd.fetch_values(attr, cover(values))) {
            visitor->consume(view(values));
        }
    }
}
//------------------------------------------------------------------------------
void compound_traverse_foward_float(
  const shared_holder<value_tree_visitor>& visitor,
  const compound& cmpd,
  const attribute& attr,
  const span_size_t value_count) {
    if(value_count == 1) {
        float value;
        if(cmpd.fetch_values(attr, cover_one(value))) {
            visitor->consume(view_one(value));
        }
    } else {
        std::vector<float> values;
        values.resize(std_size(value_count));
        if(cmpd.fetch_values(attr, cover(values))) {
            visitor->consume(view(values));
        }
    }
}
//------------------------------------------------------------------------------
void compound_traverse_foward_string(
  const shared_holder<value_tree_visitor>& visitor,
  const compound& cmpd,
  const attribute& attr,
  const span_size_t value_count) {
    if(value_count == 1) {
        std::string value;
        if(cmpd.fetch_values(attr, cover_one(value))) {
            string_view strview{value};
            visitor->consume(view_one(strview));
        }
    } else {
        std::vector<std::string> values;
        values.resize(std_size(value_count));
        if(cmpd.fetch_values(attr, cover(values))) {
            std::vector<string_view> strviews;
            strviews.resize(values.size());
            visitor->consume(view(strviews));
        }
    }
}
//------------------------------------------------------------------------------
void compound::_traverse(
  const shared_holder<value_tree_visitor>& visitor,
  const attribute& attr) const {
    const auto nc{nested_count(attr)};
    for(span_size_t idx = 0; idx < nc; ++idx) {
        auto child{nested(attr, idx)};
        const auto name{attribute_name(child)};
        if(not is_link(child) and visitor->should_continue()) {
            if(idx == 0) {
                if(name) {
                    visitor->begin_struct();
                } else {
                    visitor->begin_list();
                }
            }
            if(name) {
                visitor->begin_attribute(name);
            }
            _traverse(visitor, child);
            if(name) {
                visitor->finish_attribute(name);
            }
            if(idx + 1 == nc) {
                if(name) {
                    visitor->finish_struct();
                } else {
                    visitor->finish_list();
                }
            }
        }
    }
    const auto vc{value_count(attr)};
    if(vc > 0) {
        switch(canonical_type(attr)) {
            case value_type::bool_type:
                compound_traverse_foward_bool(visitor, *this, attr, vc);
                break;
            case value_type::int16_type:
            case value_type::int32_type:
            case value_type::int64_type:
                compound_traverse_foward_int(visitor, *this, attr, vc);
                break;
            case value_type::float_type:
                compound_traverse_foward_float(visitor, *this, attr, vc);
                break;
            case value_type::string_type:
                compound_traverse_foward_string(visitor, *this, attr, vc);
                break;
            default:
                // TODO
                break;
        }
    }
}
//------------------------------------------------------------------------------
void compound::traverse(const shared_holder<value_tree_visitor>& visitor) const {
    if(visitor) {
        visitor->begin();
        _traverse(visitor, structure());
        visitor->finish();
    }
}
//------------------------------------------------------------------------------
void compound::traverse(const visit_handler visit) const {
    const auto adapted{
      [visit](
        const compound& c,
        const attribute& a,
        const basic_string_path& p,
        span<const attribute>) -> bool {
          return visit(c, a, p);
      }};
    traverse(stack_visit_handler{construct_from, adapted});
}
//------------------------------------------------------------------------------
auto compound::make_url_str(const attribute& root) const -> std::string {
    const auto fetch_child{[&root, this](string_view name, auto& dest) {
        return fetch_value(nested(root, name), dest);
    }};
    std::string temp1;
    std::string temp2;
    std::string result;
    if(fetch_child("scheme", temp1)) {
        result.append(temp1);
        result.append("://");
    }
    if(fetch_child("domain", temp2)) {
        if(fetch_child("login", temp1)) {
            result.append(temp1);
            if(fetch_child("password", temp1)) {
                result.append(":");
                result.append(temp1);
            }
            result.append("@");
        }
        result.append(temp2);
    }
    std::uint16_t port{0};
    if(fetch_child("port", port)) {
        result.append(":");
        result.append(std::to_string(port));
    }
    if(fetch_child("path", temp1)) {
        result.append(temp1);
    }
    const auto args{nested(root, "args")};
    bool first_arg = true;
    for(auto arg_idx : integer_range(nested_count(args))) {
        const auto arg{nested(args, arg_idx)};
        if(fetch_value(arg, temp1)) {
            if(first_arg) {
                result.append("?");
                first_arg = false;
            } else {
                result.append("&");
            }
            result.append(arg.name());
            result.append("=");
            result.append(url::encode_component(temp1));
        }
    }
    if(fetch_child("fragment", temp1)) {
        result.append("#");
        result.append(temp1);
    }
    return result;
}
//------------------------------------------------------------------------------
auto compound::make_url_str(const basic_string_path& path) const
  -> std::string {
    return make_url_str(find(path));
}
//------------------------------------------------------------------------------
auto compound::get_url(const attribute& attr) const noexcept -> url {
    if(auto str{get<std::string>(attr)}) {
        if(url locator{std::move(*str)}) {
            return locator;
        }
    }
    return url{make_url_str(attr)};
}
//------------------------------------------------------------------------------
auto compound::get_url(const basic_string_path& path) const noexcept -> url {
    if(auto str{get<std::string>(path)}) {
        if(url locator{std::move(*str)}) {
            return locator;
        }
    }
    return url{make_url_str(path)};
}
//------------------------------------------------------------------------------
} // namespace eagine::valtree
