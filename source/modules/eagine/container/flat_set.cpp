/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.container:flat_set;

import eagine.core.types;
import <algorithm>;
import <functional>;
import <stdexcept>;
import <utility>;
import <vector>;

namespace eagine {
//------------------------------------------------------------------------------
export template <
  typename Key,
  typename Compare = std::less<Key>,
  class Container = std::vector<Key>>
class flat_set : private Compare {

    Container _vec;

public:
    using key_type = Key;
    using value_type = Key;
    using size_type = typename Container::size_type;
    using difference_type = typename Container::difference_type;
    using iterator = typename Container::const_iterator;
    using const_iterator = typename Container::const_iterator;
    using reference = value_type&;
    using const_reference = const value_type&;
    using allocator_type = typename Container::allocator_type;

    flat_set() noexcept = default;
    flat_set(const flat_set&) = default;
    flat_set(flat_set&&) noexcept = default;
    auto operator=(const flat_set&) -> flat_set& = default;
    auto operator=(flat_set&&) noexcept -> flat_set& = default;
    ~flat_set() noexcept = default;

    flat_set(std::initializer_list<Key> il) {
        assign(il);
    }

    flat_set(const Container& v) {
        assign(v);
    }

    void assign(std::initializer_list<Key> il) {
        _vec = Container(il);
        std::sort(_vec.begin(), _vec.end(), value_comp());
    }

    void assign(const Container& v) {
        _vec = Container(v.begin(), v.end());
        std::sort(_vec.begin(), _vec.end(), value_comp());
    }

    [[nodiscard]] auto key_comp() const noexcept -> const Compare& {
        return *this;
    }

    [[nodiscard]] auto value_comp() const noexcept -> const Compare& {
        return *this;
    }

    [[nodiscard]] auto empty() const noexcept {
        return _vec.empty();
    }

    [[nodiscard]] auto size() const noexcept {
        return _vec.size();
    }

    [[nodiscard]] auto max_size() const noexcept {
        return _vec.max_size();
    }

    [[nodiscard]] auto begin() -> iterator {
        return _vec.begin();
    }

    [[nodiscard]] auto begin() const -> const_iterator {
        return _vec.begin();
    }

    [[nodiscard]] auto end() -> iterator {
        return _vec.end();
    }

    [[nodiscard]] auto end() const -> const_iterator {
        return _vec.end();
    }

    [[nodiscard]] auto lower_bound(const Key& key) const noexcept {
        return ::std::lower_bound(begin(), end(), key, value_comp());
    }

    [[nodiscard]] auto upper_bound(const Key& key) const noexcept {
        return ::std::upper_bound(begin(), end(), key, value_comp());
    }

    [[nodiscard]] auto equal_range(const Key& key) const noexcept {
        return ::std::equal_range(begin(), end(), key, value_comp());
    }

    [[nodiscard]] auto find(const Key& key) const noexcept {
        auto [p, i] = _find_insert_pos(key);
        return i ? _vec.end() : p;
    }

    [[nodiscard]] auto contains(const Key& key) const noexcept {
        return not _find_insert_pos(key).second;
    }

    auto clear() noexcept {
        _vec.clear();
    }

    auto insert(const value_type& value) -> std::pair<iterator, bool> {
        auto ip = _find_insert_pos(value);
        ip = _do_insert(ip, value);
        return {ip.first, ip.second};
    }

    auto insert(iterator p, const value_type& value) -> iterator {
        const auto ip = _find_insert_pos(p, value);
        return _do_insert(ip, value).first;
    }

    auto erase(iterator p) -> iterator {
        return _vec.erase(p);
    }

    auto erase(iterator f, iterator t) -> iterator {
        return _vec.erase(f, t);
    }

    auto erase(const Key& key) -> size_type {
        const auto p =
          std::equal_range(_vec.begin(), _vec.end(), key, key_comp());
        const auto res = size_type(std::distance(p.first, p.second));
        assert(res <= 1);
        _vec.erase(p.first, p.second);
        return res;
    }

private:
    auto _find_insert_pos(const Key& k) const noexcept {
        const auto b = _vec.begin();
        const auto e = _vec.end();
        const auto p = std::lower_bound(b, e, k, key_comp());

        return std::pair{p, (p == e) or (k != *p)};
    }

    template <typename I>
    auto _find_insert_pos(I p, const Key& k) const noexcept {
        auto b = _vec.begin();
        auto e = _vec.end();
        if(p == e) {
            if(_vec.empty() or value_comp()(_vec.back(), k)) {
                return std::pair{p, true};
            }
            p = std::lower_bound(b, e, k, key_comp());
        }
        if(k == *p) {
            return std::pair{p, false};
        }
        if(key_comp()(k, *p)) {
            if(p != b) {
                p = std::lower_bound(b, p, k, key_comp());
            }
        } else {
            p = std::lower_bound(p, e, k, key_comp());
        }
        return std::pair{p, true};
    }

    template <typename I>
    auto _do_insert(I ip, const Key& value) -> I {
        if(ip.second) {
            ip.first = _vec.insert(ip.first, value);
        }
        return ip;
    }
};
//------------------------------------------------------------------------------
} // namespace eagine
