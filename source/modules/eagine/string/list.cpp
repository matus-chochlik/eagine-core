/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.string:list;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.valid_if;
import :multi_byte;

namespace eagine {
namespace string_list {
//------------------------------------------------------------------------------
auto encode_length(const span_size_t len) noexcept -> std::string {
    using namespace multi_byte;
    return extract(encode_code_point(code_point_t(len)));
}
//------------------------------------------------------------------------------
auto element_header_size(const string_view elem) noexcept -> span_size_t {
    using namespace multi_byte;
    return extract_or(decode_sequence_length(make_cbyte_span(elem)), 0);
}
//------------------------------------------------------------------------------
auto element_value_size(const string_view elem, const span_size_t l) noexcept
  -> span_size_t {
    using namespace multi_byte;
    return extract_or(do_decode_code_point(make_cbyte_span(elem), l), 0U);
}
//------------------------------------------------------------------------------
auto element_value_size(const string_view elem) noexcept -> span_size_t {
    return element_value_size(elem, elem.size());
}
//------------------------------------------------------------------------------
auto rev_seek_header_start(const string_view elem) noexcept -> span_size_t {
    for(auto i = elem.rbegin(); i != elem.rend(); ++i) {
        if(multi_byte::is_valid_head_byte(byte(*i))) {
            return elem.rend() - i - 1;
        }
    }
    return 0;
}
//------------------------------------------------------------------------------
export [[nodiscard]] auto front_value(const string_view list) noexcept
  -> string_view {
    const span_size_t k = element_header_size(list);
    const span_size_t l = element_value_size(list, k);
    return slice(list, k, l);
}
//------------------------------------------------------------------------------
export [[nodiscard]] auto back_value(const string_view list) noexcept
  -> string_view {
    const span_size_t i = rev_seek_header_start(list);
    const string_view header = skip(list, i);
    const span_size_t k = element_header_size(header);
    const span_size_t l = element_value_size(header, k);
    return slice(list, i - l, l);
}
//------------------------------------------------------------------------------
export [[nodiscard]] auto without_back(const string_view list) noexcept
  -> string_view {
    const span_size_t i = rev_seek_header_start(list);
    const string_view header = skip(list, i);
    const span_size_t k = element_header_size(header);
    const span_size_t l = element_value_size(header, k);
    assert(i >= k + l);
    return head(list, i - k - l);
}
//------------------------------------------------------------------------------
export auto pop_back(std::string& list) noexcept -> std::string& {
    list.resize(without_back(list).std_size());
    return list;
}
//------------------------------------------------------------------------------
export auto push_back(std::string& list, const string_view value) noexcept
  -> std::string& {
    const span_size_t vl = value.size();
    const std::string elen = encode_length(vl);
    const std::size_t nl = safe_add(list.size(), elen.size() * 2, std_size(vl));
    if(list.capacity() < nl) {
        list.reserve(nl);
    }
    list.append(elen);
    list.append(value.data(), integer(vl));
    list.append(elen);
    return list;
}
//------------------------------------------------------------------------------
export class element : public string_view {
private:
    auto _base() noexcept -> string_view {
        return *this;
    }
    auto _base() const noexcept -> string_view {
        return *this;
    }

    static inline auto _fit(const string_view s) noexcept -> string_view {
        const span_size_t hs = element_header_size(s);
        const span_size_t vs = element_value_size(s, hs);
        assert(s.size() >= hs + vs + hs);
        return {s.data(), hs + vs + hs};
    }

    static inline auto _fit(const char* ptr, const span_size_t max_size) noexcept
      -> string_view {
        return _fit(string_view(ptr, max_size));
    }

    static inline auto _rev_fit(
      const string_view s,
      [[maybe_unused]] const span_size_t rev_sz) noexcept -> string_view {
        const span_size_t hs = element_header_size(s);
        const span_size_t vs = element_value_size(s, hs);
        assert(rev_sz >= hs + vs);
        return {s.data() - hs - vs, hs + vs + hs};
    }

    static inline auto _rev_fit(
      const char* ptr,
      const span_size_t rev_sz,
      const span_size_t foot_sz) noexcept -> string_view {
        return _rev_fit(string_view(ptr, foot_sz), rev_sz);
    }

public:
    element(const char* ptr, const span_size_t max_size) noexcept
      : string_view(_fit(ptr, max_size)) {}

    element(
      const char* ptr,
      const span_size_t rev_sz,
      const span_size_t foot_sz) noexcept
      : string_view(_rev_fit(ptr, rev_sz, foot_sz)) {}

    [[nodiscard]] auto header_size() const noexcept -> span_size_t {
        return element_header_size(_base());
    }

    [[nodiscard]] auto header() const noexcept -> string_view {
        return {data(), header_size()};
    }

    [[nodiscard]] auto value_size() const noexcept -> span_size_t {
        return element_value_size(header());
    }

    [[nodiscard]] auto value_data() const noexcept -> const char* {
        return data() + header_size();
    }

    [[nodiscard]] auto value() const noexcept -> string_view {
        return {value_data(), value_size()};
    }

    [[nodiscard]] auto footer_size() const noexcept -> span_size_t {
        return element_header_size(_base());
    }

    [[nodiscard]] auto footer() const noexcept -> string_view {
        return {data() + safe_add(header_size(), value_size()), header_size()};
    }
};
//------------------------------------------------------------------------------
export template <typename Func>
void for_each_elem(const string_view list, Func func) noexcept {
    span_size_t i = 0;
    bool first = true;
    while((i + 2) <= list.size()) {
        element elem(list.data() + i, list.size() - i);
        func(elem, first);
        i += elem.size();
        first = false;
    }
}
//------------------------------------------------------------------------------
export template <typename Func>
void for_each(const string_view list, Func func) noexcept {
    const auto adapted_func = [&func](const element& elem, bool) {
        func(elem.value());
    };
    for_each_elem(list, adapted_func);
}
//------------------------------------------------------------------------------
export [[nodiscard]] auto element_count(const string_view list) noexcept
  -> span_size_t {
    span_size_t result{0};
    const auto count_func = [&result](const element&, bool) {
        ++result;
    };
    for_each_elem(list, count_func);
    return result;
}
//------------------------------------------------------------------------------
export template <typename Func>
void rev_for_each_elem(const string_view list, Func func) noexcept {
    span_size_t i = list.size() - 1;
    bool first = true;
    while(i > 0) {
        while(not multi_byte::is_valid_head_byte(byte(list[i]))) {
            assert(i > 0);
            --i;
        }
        element elem(list.data() + i, i, list.size() - i);
        func(elem, first);
        i -= safe_add(elem.header_size(), elem.value_size(), 1);
        first = false;
    }
}
//------------------------------------------------------------------------------
export template <typename Func>
void rev_for_each(const string_view list, Func func) noexcept {
    auto adapted_func = [&func](const element& elem, bool) {
        func(elem.value());
    };
    rev_for_each_elem(list, adapted_func);
}
//------------------------------------------------------------------------------
auto split_into(
  std::string& dst,
  const string_view str,
  const string_view sep) noexcept -> span_size_t {
    span_size_t cnt = 0;
    for_each_delimited(str, sep, [&dst, &cnt](const auto& x) {
        push_back(dst, x);
        ++cnt;
    });
    return cnt;
}
//------------------------------------------------------------------------------
auto split(string_view str, string_view sep) noexcept
  -> std::tuple<std::string, span_size_t> {
    std::string res;
    const auto cnt = split_into(res, str, sep);
    return std::make_tuple(std::move(res), cnt);
}
//------------------------------------------------------------------------------
template <typename Func>
auto for_each_separated_c_str(const char* str, const char sep, Func func) noexcept
  -> span_size_t {
    span_size_t cnt = 0;
    const char* bgn = str;
    const char* pos = bgn;
    if(sep != '\0') {
        while(bool(str) and (*pos != '\0')) {
            if(*pos == sep) {
                if(pos - bgn > 0) {
                    func(string_view(bgn, pos - bgn));
                    ++cnt;
                    bgn = ++pos;
                } else {
                    break;
                }
            } else {
                ++pos;
            }
        }
    } else {
        while(bool(str)) {
            if((*pos == sep) or (*pos == char(0))) {
                if(pos - bgn > 1) {
                    func(string_view(bgn, pos - bgn));
                    ++cnt;
                    bgn = ++pos;
                } else {
                    break;
                }
            } else {
                ++pos;
            }
        }
    }
    return cnt;
}
//------------------------------------------------------------------------------
auto split_c_str_into(std::string& dst, const char* str, const char sep) noexcept
  -> span_size_t {
    return for_each_separated_c_str(
      str, sep, [&dst](auto elem) { push_back(dst, elem); });
}
//------------------------------------------------------------------------------
export [[nodiscard]] auto split_c_str(const char* str, const char sep) noexcept
  -> std::tuple<std::string, span_size_t> {
    std::string res;
    const auto cnt = split_c_str_into(res, str, sep);
    return std::make_tuple(std::move(res), cnt);
}
//------------------------------------------------------------------------------
export [[nodiscard]] auto join(
  const string_view list,
  const string_view sep,
  const bool trail_sep) noexcept -> std::string {
    const span_size_t slen = sep.size();
    span_size_t len = trail_sep ? slen : 0;
    const auto get_len = [&len, slen](const element& elem, bool first) {
        if(not first) {
            len += slen;
        }
        len += elem.value_size();
    };
    for_each_elem(list, get_len);

    std::string res;
    res.reserve(integer(len));

    auto fill = [&res, sep](const element& elem, bool first) {
        if(not first) {
            append_to(sep, res);
        }
        res.append(elem.value_data(), integer(elem.value_size()));
    };
    for_each_elem(list, fill);

    if(trail_sep) {
        append_to(sep, res);
    }
    assert(res.size() == std_size(len));

    return res;
}
//------------------------------------------------------------------------------
export [[nodiscard]] auto join(
  const string_view list,
  const string_view sep) noexcept -> std::string {
    return join(list, sep, false);
}
//------------------------------------------------------------------------------
export template <typename Iter>
class iterator {
public:
    using difference_type = std::ptrdiff_t;
    using value_type = string_view;
    using reference = const value_type&;
    using pointer = const value_type*;
    using iterator_category = std::forward_iterator_tag;

    iterator(Iter pos) noexcept
      : _pos{pos} {}

    [[nodiscard]] auto operator<=>(const iterator&) const noexcept = default;

    [[nodiscard]] auto operator*() const noexcept -> reference {
        _update();
        return _tmp;
    }

    [[nodiscard]] auto operator->() const noexcept -> pointer {
        _update();
        return &_tmp;
    }

    auto operator++() noexcept -> auto& {
        span_size_t ll = _len_len();
        span_size_t vl = _val_len(ll);
        _pos += ll + vl + ll;
        _tmp = string_view();
        return *this;
    }

    auto operator++(int) noexcept -> iterator {
        iterator result = *this;
        ++(*this);
        return result;
    }

private:
    Iter _pos;
    mutable string_view _tmp;

    auto _b() const noexcept -> byte {
        return byte(*_pos);
    }

    auto _len_len() const noexcept -> span_size_t {
        byte b = _b();
        assert(multi_byte::is_valid_head_byte(b));
        return extract(multi_byte::do_decode_sequence_length(b));
    }

    auto _val_len(const span_size_t ll) const noexcept -> span_size_t {
        string_view el{&*_pos, ll};
        using namespace multi_byte;
        return extract_or(do_decode_code_point(make_cbyte_span(el), ll), 0U);
    }

    void _update() const noexcept {
        if(_tmp.size() == 0) {
            span_size_t ll = _len_len();
            span_size_t vl = _val_len(ll);
            _tmp = string_view{&*(_pos + ll), vl};
        }
    }
};
//------------------------------------------------------------------------------
export template <typename Iter>
class rev_iterator {
public:
    using difference_type = std::ptrdiff_t;
    using value_type = string_view;
    using reference = const value_type&;
    using pointer = const value_type*;
    using iterator_category = std::forward_iterator_tag;

    rev_iterator(Iter pos) noexcept
      : _pos{pos} {}

    [[nodiscard]] auto operator<=>(const rev_iterator&) const noexcept = default;

    [[nodiscard]] auto operator*() const noexcept -> reference {
        _update();
        return _tmp;
    }

    [[nodiscard]] auto operator->() const noexcept -> pointer {
        _update();
        return &_tmp;
    }

    auto operator++() noexcept -> auto& {
        _rseek_head();
        span_size_t ll = _len_len();
        span_size_t vl = _val_len(ll);
        _pos -= ll + vl + 1;
        _tmp = string_view();
        return *this;
    }

    auto operator++(int) noexcept -> rev_iterator {
        rev_iterator result = *this;
        ++(*this);
        return result;
    }

private:
    mutable Iter _pos;
    mutable string_view _tmp;

    auto _b() const noexcept -> byte {
        return byte(*_pos);
    }

    void _rseek_head() const noexcept {
        while(not multi_byte::is_valid_head_byte(_b())) {
            --_pos;
        }
    }

    auto _len_len() const noexcept -> span_size_t {
        byte b = _b();
        assert(multi_byte::is_valid_head_byte(b));
        return extract(multi_byte::do_decode_sequence_length(b));
    }

    auto _val_len(span_size_t ll) const noexcept -> span_size_t {
        string_view el{&*_pos, ll};
        using namespace multi_byte;
        return extract_or(do_decode_code_point(make_cbyte_span(el), ll), 0U);
    }

    void _update() const noexcept {
        if(_tmp.size() == 0) {
            _rseek_head();
            span_size_t ll = _len_len();
            span_size_t vl = _val_len(ll);
            _tmp = string_view{&*(_pos - vl), vl};
        }
    }
};
//------------------------------------------------------------------------------
export template <typename Range>
class range_view {
public:
    using iterator = eagine::string_list::iterator<typename Range::iterator>;

    range_view(Range& range) noexcept
      : _range{range} {}

    [[nodiscard]] auto begin() const noexcept -> iterator {
        return {_range.begin()};
    }

    [[nodiscard]] auto end() const noexcept -> iterator {
        return {_range.end()};
    }

private:
    Range& _range;
};
//------------------------------------------------------------------------------
export template <typename Range>
class rev_range_view {
public:
    using iterator =
      eagine::string_list::rev_iterator<typename Range::iterator>;

    rev_range_view(Range& range) noexcept
      : _range{range} {}

    [[nodiscard]] auto begin() const noexcept -> iterator {
        return {_range.end() - 1};
    }

    [[nodiscard]] auto end() const noexcept -> iterator {
        return {_range.begin() - 1};
    }

private:
    Range& _range;
};
//------------------------------------------------------------------------------
export template <typename Range>
class basic_string_list {
public:
    using value_type = string_view;
    using iterator =
      eagine::string_list::iterator<typename Range::const_iterator>;
    using reverse_iterator =
      eagine::string_list::rev_iterator<typename Range::const_iterator>;

    basic_string_list() noexcept = default;

    basic_string_list(Range range) noexcept
      : _range{std::move(range)} {}

    [[nodiscard]] auto begin() const noexcept -> iterator {
        return {_range.begin()};
    }

    [[nodiscard]] auto end() const noexcept -> iterator {
        return {_range.end()};
    }

    [[nodiscard]] auto rbegin() const noexcept -> reverse_iterator {
        return {_range.end() - 1};
    }

    [[nodiscard]] auto rend() const noexcept -> reverse_iterator {
        return {_range.begin() - 1};
    }

private:
    Range _range{};
};
//------------------------------------------------------------------------------
} // namespace string_list
//------------------------------------------------------------------------------
export [[nodiscard]] auto make_string_list(std::string str)
  -> string_list::basic_string_list<std::string> {
    return {std::move(str)};
}
//------------------------------------------------------------------------------
export [[nodiscard]] auto split_into_string_list(
  const string_view src,
  const char sep) noexcept {
    std::string temp;
    string_list::split_into(temp, src, view_one(sep));
    return make_string_list(std::move(temp));
}
//------------------------------------------------------------------------------
export [[nodiscard]] auto split_c_str_into_string_list(
  const char* src,
  const char sep) noexcept {
    std::string temp;
    string_list::split_c_str_into(temp, src, sep);
    return make_string_list(std::move(temp));
}
//------------------------------------------------------------------------------
} // namespace eagine
