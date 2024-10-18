/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module eagine.core.runtime;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.string;
import eagine.core.utility;
import eagine.core.identifier;

namespace eagine {
//------------------------------------------------------------------------------
// query args
//------------------------------------------------------------------------------
url_query_args::url_query_args(url& parent) noexcept
  : _parent{parent} {}
//------------------------------------------------------------------------------
auto url_query_args::all() const noexcept
  -> generator<std::tuple<string_view, string_view>> {
    return _parent->_all_args();
}
//------------------------------------------------------------------------------
auto url_query_args::arg_value(const string_view name) const noexcept
  -> optionally_valid<string_view> {
    return _parent->_arg_value(name);
}
//------------------------------------------------------------------------------
auto url_query_args::decoded_arg_value(const string_view name) const noexcept
  -> optionally_valid<std::string> {
    return arg_value(name).and_then(url::decode_component);
}
//------------------------------------------------------------------------------
auto url_query_args::arg_url(const string_view name) const noexcept -> url {
    return decoded_arg_value(name).transform(_1.cast_to<url>()).or_default();
}
//------------------------------------------------------------------------------
auto url_query_args::arg_identifier(const string_view name) const noexcept
  -> identifier {
    if(const auto str{arg_value(name)}) {
        if(identifier::can_be_encoded(*str)) {
            return identifier{*str};
        }
    }
    return {};
}
//------------------------------------------------------------------------------
// url
//------------------------------------------------------------------------------
auto url::encode_component(const string_view src) noexcept -> std::string {
    const auto is_valid{[](char c) {
        return ((c >= 'a') and (c <= 'z')) or ((c >= 'A') and (c <= 'Z')) or
               ((c >= '0') and (c <= '9')) or (c == '-') or (c == '_') or
               (c == '.') or (c == '~');
    }};
    std::string result;
    result.reserve(std_size(src.size() * 2));
    for(const char c : src) {
        if(is_valid(c)) {
            result.push_back(c);
        } else {
            const std::string_view hd{"0123456789ABCDEF"};
            result.push_back('%');
            result.push_back(hd[(byte(c) >> 4U) & 0x0FU]);
            result.push_back(hd[byte(c) & 0x0FU]);
        }
    }
    return result;
}
//------------------------------------------------------------------------------
static inline auto _url_decode_char(const char hi, const char lo) {
    const auto bhi{hex_char2byte(hi)};
    const auto blo{hex_char2byte(lo)};
    if(bhi and blo) {
        return char(*bhi << 4U | *blo);
    }
    return '\0';
}
//------------------------------------------------------------------------------
auto url::decode_component(const string_view src) noexcept
  -> optionally_valid<std::string> {
    std::string result;
    result.reserve(std_size(src.size()));
    int decoding{0};
    char hi{'\0'};
    for(const char c : src) {
        if(decoding == 2) {
            if(const char d{_url_decode_char(hi, c)}) {
                result.push_back(d);
                decoding = 0;
            } else {
                return {};
            }
        } else if(decoding == 1) {
            hi = c;
            decoding = 2;
        } else if(c == '%') {
            decoding = 1;
        } else {
            result.push_back(c);
        }
    }
    return {result};
}
//------------------------------------------------------------------------------
auto url::components_are_equal(const string_view l, const string_view r) noexcept
  -> bool {
    const auto make_getter{[](const string_view s) {
        return [pos{s.begin()}, end{s.end()}] mutable {
            if(pos != end) {
                const char c{*pos};
                ++pos;
                if(c != '%') {
                    return c;
                }
                if(pos == end) {
                    return c;
                }
                const char hi{*pos};
                ++pos;
                if(pos == end) {
                    return hi;
                }
                const char lo{*pos};
                ++pos;
                return _url_decode_char(hi, lo);
            }
            return '\0';
        };
    }};
    auto getl{make_getter(l)};
    auto getr{make_getter(r)};

    while(true) {
        const auto cl{getl()};
        const auto cr{getr()};

        if(cl != cr) {
            return false;
        }
        if(cl == '\0') {
            break;
        }
    }
    return true;
}
//------------------------------------------------------------------------------
auto url::_get_regex() noexcept -> const std::regex& {
    static const std::regex re{
      // clang-format off
      R"(^((([\w]+):)?\/\/)(([^:]+)(:(\S+))?@)?((((\w[\w_-]{0,62}(\.\w[\w_-]{0,62})*))|((10|127)(\.\d{1,3}){3})|((169\.254|192\.168)(\.\d{1,3}){2})|(172\.(1[6-9]|2\d|3[0-1])(\.\d{1,3}){2})|([1-9]\d?|1\d\d|2[01]\d|22[0-3])(\.(1?\d{1,2}|2[0-4]\d|25[0-5])){2}(\.([1-9]\d?|1\d\d|2[0-4]\d|25[0-4])))(:(\d{1,5}))?)?((/[\w._-]+)*/?)?(\?(([\w._]+=[^&#]*)([&]([\w_]+=[^&#]*))*))?(#([\w_-]*))?$)",
      // clang-format on
      std::regex::ECMAScript};
    return re;
}
//------------------------------------------------------------------------------
auto url::is_url(const string_view str) noexcept -> bool {
    std::match_results<string_view::iterator> match{};
    return std::regex_match(str.begin(), str.end(), match, _get_regex());
}
//------------------------------------------------------------------------------
auto url::_sw(_range r) const noexcept -> string_view {
    return string_view{
      head(skip(view(_url_str), std::get<0>(r)), std::get<1>(r))};
}
//------------------------------------------------------------------------------
auto url::_swov(_range r) const noexcept -> valid_if_not_empty<string_view> {
    return {_sw(r)};
}
//------------------------------------------------------------------------------
void url::_parse_args() noexcept {
    const string_view str{_url_str};

    _arg_list.clear();
    for_each_delimited(_sw(_query), string_view{"&"}, [&, this](auto part) {
        auto [name, value]{split_by_first(part, string_view{"="})};
        const auto n_offs{std::distance(str.data(), name.data())};
        const auto v_offs{std::distance(str.data(), value.data())};
        _arg_list.emplace_back(std::make_tuple(
          _range{n_offs, name.size()}, _range{v_offs, value.size()}));
    });
}
//------------------------------------------------------------------------------
auto url::_all_args() const noexcept
  -> generator<std::tuple<string_view, string_view>> {
    for(const auto& [name_r, value_r] : _arg_list) {
        co_yield std::make_tuple(_sw(name_r), _sw(value_r));
    }
}
//------------------------------------------------------------------------------
auto url::_arg_value(const string_view name) const noexcept
  -> optionally_valid<string_view> {
    for(const auto& [name_r, value_r] : _arg_list) {
        if(_sw(name_r) == name) {
            return {_sw(value_r)};
        }
    }
    return {};
}
//------------------------------------------------------------------------------
void url::_cover(
  url::_range& part,
  const std::match_results<std::string::iterator>& match,
  const std::size_t index) const noexcept {
    if(index < match.size()) {
        auto& m = match[index];
        part = {m.first - _url_str.begin(), m.second - m.first};
    }
}
//------------------------------------------------------------------------------
url::url(
  std::string url_str,
  std::match_results<std::string::iterator> match) noexcept
  : _url_str{std::move(url_str)}
  , _parsed{
      std::regex_match(_url_str.begin(), _url_str.end(), match, _get_regex())} {

    if(_parsed) {
        _cover(_scheme, match, 3);
        _cover(_login, match, 5);
        _cover(_passwd, match, 7);
        _cover(_host, match, 9);
        _cover(_port, match, 28);
        _cover(_path, match, 29);
        _cover(_query, match, 32);
        _cover(_fragment, match, 37);
        _parse_args();
    }
}
//------------------------------------------------------------------------------
auto url::hash_id() const noexcept -> identifier_t {
    return integer_hash<identifier_t>(std::hash<std::string>{}(_url_str));
}
//------------------------------------------------------------------------------
auto url::login() const noexcept -> optionally_valid<std::string> {
    return _swov(_login).and_then(&url::decode_component);
}
//------------------------------------------------------------------------------
auto url::password() const noexcept -> optionally_valid<std::string> {
    return _swov(_passwd).and_then(&url::decode_component);
}
//------------------------------------------------------------------------------
auto url::has_domain(const string_view str) const noexcept -> bool {
    return components_are_equal(str, _sw(_host));
}
//------------------------------------------------------------------------------
auto url::domain_identifier() const noexcept -> identifier {
    if(const auto str{_sw(_host)}) {
        if(identifier::can_be_encoded(str)) {
            return identifier{str};
        }
    }
    return {};
}
//------------------------------------------------------------------------------
auto url::path_dirname() const noexcept -> valid_if_not_empty<string_view> {
    return {slice_before_last(_sw(_path), string_view{"/"})};
}
//------------------------------------------------------------------------------
auto url::path_basename() const noexcept -> valid_if_not_empty<string_view> {
    return {slice_after_last(_sw(_path), string_view{"/"})};
}
//------------------------------------------------------------------------------
auto url::has_path_prefix(const string_view prefix) const noexcept -> bool {
    return starts_with(_sw(_path), prefix);
}
//------------------------------------------------------------------------------
auto url::has_path_suffix(const string_view suffix) const noexcept -> bool {
    return ends_with(_sw(_path), suffix);
}
//------------------------------------------------------------------------------
auto url::path() const noexcept -> basic_string_path {
    return {basic_string_path{_sw(_path), split_by, "/"}};
}
//------------------------------------------------------------------------------
auto url::has_path(const string_view str) const noexcept -> bool {
    return components_are_equal(str, _sw(_path)) or
           (not str.empty() and str.front() != '/' and
            components_are_equal(str, skip(_sw(_path), 1)));
}
//------------------------------------------------------------------------------
auto url::path_identifier() const noexcept -> identifier {
    if(const string_view str{skip(_sw(_path), 1)}) {
        if(identifier::can_be_encoded(str)) {
            return identifier{str};
        }
    }
    return {};
}
//------------------------------------------------------------------------------
auto url::argument(const string_view arg_name) const noexcept
  -> valid_if_not_empty<string_view> {
    string_view result;
    for_each_delimited(_sw(_query), string_view{"+"}, [&](auto part) {
        auto [name, value] = split_by_first(part, string_view{"="});
        if(not result and are_equal(name, arg_name)) {
            result = value;
        }
    });
    return {result};
}
//------------------------------------------------------------------------------
} // namespace eagine
