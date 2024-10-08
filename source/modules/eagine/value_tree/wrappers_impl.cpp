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
void compound::traverse(const shared_holder<value_tree_visitor>& visitor) const {
    if(visitor and visitor->should_continue()) {
        std::vector<attribute> atr;
        std::vector<span_size_t> pos;
        bool struct_otherwise_list{false};

        atr.push_back(structure());
        pos.push_back(0);
        visitor->begin();

        bool bool_value{false};
        std::vector<byte> byte_values;
        std::vector<std::int64_t> int64_values;
        std::vector<float> float_values;

        while(true) {
            assert(atr.size() == pos.size());
            const auto nc{nested_count(atr.back())};

            if(pos.back() < nc) {
                auto child{nested(atr.back(), pos.back())};
                const auto name{attribute_name(child)};
                if(name) {
                    struct_otherwise_list = true;
                } else {
                    struct_otherwise_list = false;
                }
                if(not is_link(child) and visitor->should_continue()) {
                    if(pos.back() == 0) {
                        if(struct_otherwise_list) {
                            visitor->begin_struct();
                        } else {
                            visitor->begin_list();
                        }
                    }
                    if(name) {
                        visitor->begin_attribute(name);
                    }
                    atr.push_back(child);
                    pos.push_back(0);
                    continue;
                }
            } else if(nc == 0) {
                if(visitor->should_continue()) {
                    switch(canonical_type(atr.back())) {
                        case value_type::bool_type:
                            if(value_count(atr.back()) == 1) {
                                if(fetch_values(
                                     atr.back(), cover_one(bool_value))) {
                                    visitor->consume(view_one(bool_value));
                                }
                            } else {
                                // TODO
                            }
                            break;
                        case value_type::byte_type:
                            byte_values.resize(
                              std_size(value_count(atr.back())));
                            if(fetch_values(atr.back(), cover(byte_values))) {
                                memory::const_block blob{view(byte_values)};
                                visitor->unparsed_data(view_one(blob));
                            }
                            break;
                        case value_type::int16_type:
                            int64_values.resize(
                              std_size(value_count(atr.back())));
                            if(fetch_values(atr.back(), cover(int64_values))) {
                                visitor->consume(view(int64_values));
                            }
                            break;
                        case value_type::int32_type:
                            int64_values.resize(
                              std_size(value_count(atr.back())));
                            if(fetch_values(atr.back(), cover(int64_values))) {
                                visitor->consume(view(int64_values));
                            }
                            break;
                        case value_type::int64_type:
                            int64_values.resize(
                              std_size(value_count(atr.back())));
                            if(fetch_values(atr.back(), cover(int64_values))) {
                                visitor->consume(view(int64_values));
                            }
                            break;
                        case value_type::float_type:
                            float_values.resize(
                              std_size(value_count(atr.back())));
                            if(fetch_values(atr.back(), cover(float_values))) {
                                visitor->consume(view(float_values));
                            }
                            break;
                        case value_type::duration_type:
                            // TODO
                            break;
                        case value_type::string_type:
                            break;
                        default:
                            break;
                    }
                }
                atr.pop_back();
                pos.pop_back();
            } else {
                if(struct_otherwise_list) {
                    visitor->finish_struct();
                } else {
                    visitor->finish_list();
                }
                atr.pop_back();
                pos.pop_back();
            }
            if(pos.empty()) {
                break;
            }
            ++pos.back();
        }
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
