/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module eagine.core.runtime;

import std;
import eagine.core.types;
import eagine.core.memory;

namespace eagine {
//------------------------------------------------------------------------------
auto url::encode_component(const string_view src) noexcept -> std::string {
    const auto is_valid{[](char c) {
        return ((c >= 'a') and (c <= 'z')) or ((c >= 'A') and (c <= 'Z')) or
               ((c >= '0') and (c <= '0')) or (c == '-') or (c == '_') or
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
auto url::decode_component(const string_view src) noexcept
  -> optionally_valid<std::string> {
    std::string result;
    result.reserve(std_size(src.size()));
    const auto from_hex{[](const char c) -> byte {
        if((c >= '0') and (c <= '9')) {
            return byte(c - '0');
        }
        if((c >= 'A') and (c <= 'F')) {
            return byte(c - 'A' + 10);
        }
        if((c >= 'a') and (c <= 'f')) {
            return byte(c - 'a' + 10);
        }
        return 0xFFU;
    }};
    const auto decode{[&](const char hi, const char lo) {
        const auto bhi{from_hex(hi)};
        const auto blo{from_hex(lo)};
        if(bhi != 0x0FFU and blo != 0xFFU) {
            return char(bhi << 4U | blo);
        }
        return '\0';
    }};
    int decoding{0};
    char hi{'\0'};
    for(const char c : src) {
        if(decoding == 2) {
            if(const char d{decode(hi, c)}) {
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
auto url::_get_regex() noexcept -> const std::regex& {
    static const std::regex re{
      // clang-format off
      R"(^((([\w]+):)?\/\/)(([^:]+)(:(\S+))?@)?((((\w[\w_-]{0,62}(\.\w[\w_-]{0,62})*))|((10|127)(\.\d{1,3}){3})|((169\.254|192\.168)(\.\d{1,3}){2})|(172\.(1[6-9]|2\d|3[0-1])(\.\d{1,3}){2})|([1-9]\d?|1\d\d|2[01]\d|22[0-3])(\.(1?\d{1,2}|2[0-4]\d|25[0-5])){2}(\.([1-9]\d?|1\d\d|2[0-4]\d|25[0-4])))(:(\d{1,5}))?)?((/[\w._-]+)*/?)?(\?(([\w._]+=[^+#]*)(\+([\w_]+=[^+#]*))*))?(#([\w_-]*))?$)",
      // clang-format on
      std::regex::ECMAScript};
    return re;
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
        _query_args = _parse_args();
    }
}
//------------------------------------------------------------------------------
} // namespace eagine
