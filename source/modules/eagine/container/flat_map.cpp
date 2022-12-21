/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.container:flat_map;

import eagine.core.types;
import <algorithm>;
import <functional>;
import <stdexcept>;
import <utility>;
export import <vector>;

namespace eagine {
//------------------------------------------------------------------------------
template <typename Key, typename Val, typename Cmp>
struct flat_map_value_compare : Cmp {

    constexpr auto key_comp() const noexcept -> const Cmp& {
        return *this;
    }

    template <typename L, typename R>
    constexpr auto operator()(
      const std::pair<L, Val>& a,
      const std::pair<R, Val>& b) const noexcept -> bool {
        return key_comp()(a.first, b.first);
    }

    template <typename L, typename R>
    constexpr auto operator()(const std::pair<L, Val>& a, const R& b)
      const noexcept {
        return key_comp()(a.first, b);
    }

    template <typename L, typename R>
    constexpr auto operator()(const L& a, const std::pair<R, Val>& b)
      const noexcept {
        return key_comp()(a, b.first);
    }
};
//------------------------------------------------------------------------------
export template <typename Key, typename Val, typename Cmp>
struct flat_map_ops : flat_map_value_compare<Key, Val, Cmp> {
    using value_type = std::pair<const Key, Val>;

    constexpr auto value_comp() const noexcept
      -> const flat_map_value_compare<Key, Val, Cmp>& {
        return *this;
    }

    template <typename I>
    static auto empty(const I b, const I e) noexcept {
        return b == e;
    }

    template <typename I>
    static auto size(const I b, const I e) noexcept {
        using ::std::distance;
        return static_cast<span_size_t>(distance(b, e));
    }

    template <typename I, typename K>
    auto lower_bound(const I b, const I e, const K& key) const noexcept {
        return ::std::lower_bound(b, e, key, value_comp());
    }

    template <typename I, typename K>
    auto upper_bound(const I b, const I e, const K& key) const noexcept {
        return ::std::upper_bound(b, e, key, value_comp());
    }

    template <typename I, typename K>
    auto equal_range(const I b, const I e, const K& key) const noexcept {
        return ::std::equal_range(b, e, key, value_comp());
    }

    template <typename I, typename K>
    auto find(I b, const I e, const K& key) const noexcept -> I {
        b = lower_bound(b, e, key);
        if((b != e) and value_comp()(key, *b)) {
            b = e;
        }
        return b;
    }

    template <typename I, typename K>
    auto at(const I b, const I e, const K& key) const -> auto& {
        const auto p = find(b, e, key);
        if(p == e) {
            throw std::out_of_range("Invalid flat map key");
        }
        return p->second;
    }

    template <typename I, typename K>
    auto get(const I b, const I e, const K& key) const noexcept -> auto& {
        const auto p = find(b, e, key);
        assert(p != e);
        return p->second;
    }
};
//------------------------------------------------------------------------------
export template <typename Key, typename Val, typename Cmp, typename Derived>
class flat_map_view_crtp : flat_map_ops<Key, Val, Cmp> {
    using _ops_t = flat_map_ops<Key, Val, Cmp>;

public:
    using key_type = Key;
    using mapped_type = Val;
    using value_type = std::pair<const Key, Val>;
    using reference = value_type&;
    using const_reference = const value_type&;

    using key_compare = Cmp;
    using value_compare = flat_map_value_compare<Key, Val, Cmp>;

    [[nodiscard]] auto key_comp() const noexcept -> const key_compare& {
        return _ops().value_comp().key_comp();
    }

    [[nodiscard]] auto value_comp() const noexcept -> const value_compare& {
        return _ops().value_comp();
    }

    [[nodiscard]] auto empty() const noexcept -> bool {
        return _ops().empty(_b(), _e());
    }

    [[nodiscard]] auto size() const noexcept {
        return _ops().size(_b(), _e());
    }

    template <typename K>
    [[nodiscard]] auto find(const K& key) noexcept {
        return _ops().find(_b(), _e(), key);
    }

    template <typename K>
    [[nodiscard]] auto find(const K& key) const noexcept {
        return _ops().find(_b(), _e(), key);
    }

    template <typename K>
    [[nodiscard]] auto contains(const K& key) const noexcept {
        return _ops().find(_b(), _e(), key) != _e();
    }

    template <typename K>
    [[nodiscard]] auto lower_bound(const K& key) noexcept {
        return _ops().lower_bound(_b(), _e(), key);
    }

    template <typename K>
    [[nodiscard]] auto lower_bound(const K& key) const noexcept {
        return _ops().lower_bound(_b(), _e(), key);
    }

    template <typename K>
    [[nodiscard]] auto upper_bound(const K& key) noexcept {
        return _ops().upper_bound(_b(), _e(), key);
    }

    template <typename K>
    [[nodiscard]] auto upper_bound(const K& key) const noexcept {
        return _ops().upper_bound(_b(), _e(), key);
    }

    template <typename K>
    [[nodiscard]] auto equal_range(const K& key) noexcept {
        return _ops().equal_range(_b(), _e(), key);
    }

    template <typename K>
    [[nodiscard]] auto equal_range(const K& key) const noexcept {
        return _ops().equal_range(_b(), _e(), key);
    }

    template <typename K>
    [[nodiscard]] auto at(const K& key) -> Val& {
        return _ops().at(_b(), _e(), key);
    }

    template <typename K>
    [[nodiscard]] auto at(const K& key) const -> const Val& {
        return _ops().at(_b(), _e(), key);
    }

protected:
    constexpr auto _ops() const noexcept -> const _ops_t& {
        return *this;
    }

private:
    auto _self() const noexcept -> const Derived& {
        return *static_cast<const Derived*>(this);
    }
    auto _self() noexcept -> Derived& {
        return *static_cast<Derived*>(this);
    }

    auto _b() const noexcept {
        return _self().begin();
    }
    auto _b() noexcept {
        return _self().begin();
    }

    auto _e() const noexcept {
        return _self().end();
    }
    auto _e() noexcept {
        return _self().end();
    }
};
//------------------------------------------------------------------------------
export template <
  typename Key,
  typename Val,
  typename Cmp = std::less<Key>,
  typename Container = std::vector<std::pair<Key, Val>>>
class flat_map
  : public flat_map_view_crtp<Key, Val, Cmp, flat_map<Key, Val, Cmp, Container>> {
    using _base =
      flat_map_view_crtp<Key, Val, Cmp, flat_map<Key, Val, Cmp, Container>>;
    using _base::_ops;

    Container _vec{};

public:
    using typename _base::key_compare;
    using typename _base::key_type;
    using typename _base::mapped_type;
    using typename _base::value_type;
    using size_type = typename Container::size_type;
    using difference_type = typename Container::difference_type;
    using iterator = typename Container::iterator;
    using const_iterator = typename Container::const_iterator;
    using allocator_type = typename Container::allocator_type;

    using _base::key_comp;
    using _base::lower_bound;
    using _base::value_comp;

    flat_map() noexcept = default;
    flat_map(const flat_map&) = default;
    flat_map(flat_map&&) noexcept = default;
    auto operator=(const flat_map&) -> flat_map& = default;
    auto operator=(flat_map&&) noexcept -> flat_map& = default;
    ~flat_map() noexcept = default;

    flat_map(std::initializer_list<std::pair<Key, Val>> il) {
        assign(il);
    }

    flat_map(const std::vector<value_type>& v) {
        assign(v);
    }

    void assign(std::initializer_list<std::pair<Key, Val>> il) {
        _vec.assign(il.begin(), il.end());
        std::sort(_vec.begin(), _vec.end(), value_comp());
    }

    void assign(const std::vector<value_type>& v) {
        _vec.assign(v.begin(), v.end());
        std::sort(_vec.begin(), _vec.end(), value_comp());
    }

    [[nodiscard]] auto empty() const noexcept -> bool {
        return _vec.empty();
    }

    [[nodiscard]] auto size() const noexcept -> size_type {
        return _vec.size();
    }

    [[nodiscard]] auto max_size() const noexcept -> size_type {
        return _vec.max_size();
    }

    auto reserve(const size_type sz) -> auto& {
        _vec.reserve(sz);
        return *this;
    }

    auto clear() -> auto& {
        _vec.clear();
        return *this;
    }

    [[nodiscard]] auto front() const noexcept -> auto& {
        return _vec.front();
    }

    [[nodiscard]] auto back() const noexcept -> auto& {
        return _vec.back();
    }

    [[nodiscard]] auto begin() noexcept -> iterator {
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

    [[nodiscard]] auto operator[](const Key& key) -> auto& {
        auto ip = _find_insert_pos(key);
        ip = _do_emplace(ip, key);
        return ip.first->second;
    }

    template <typename... Args>
    auto emplace(const Key& key, Args&&... args) -> std::pair<iterator, bool> {
        auto ip = _find_insert_pos(key);
        ip = _do_emplace(ip, key, std::forward<Args>(args)...);
        return ip;
    }

    template <typename... Args>
    auto try_emplace(const Key& key, Args&&... args)
      -> std::pair<iterator, bool> {
        auto ip = _find_insert_pos(key);
        ip = _do_emplace(ip, key, std::forward<Args>(args)...);
        return ip;
    }

    [[nodiscard]] auto find_insert_position(const key_type& key)
      -> std::pair<iterator, bool> {
        return _find_insert_pos(key);
    }

    auto insert(const value_type& value) -> std::pair<iterator, bool> {
        auto ip = _find_insert_pos(value.first);
        ip = _do_insert(ip, value);
        return {ip.first, ip.second};
    }

    auto insert(iterator p, const value_type& value) -> iterator {
        const auto ip = _find_insert_pos(p, value.first);
        return _do_insert(ip, value).first;
    }

    auto erase(iterator p) -> iterator {
        return _vec.erase(p);
    }

    auto erase(iterator f, iterator t) -> iterator {
        return _vec.erase(f, t);
    }

    template <typename K>
    auto erase(const K& key) -> size_type {
        const auto p = _ops().equal_range(_vec.begin(), _vec.end(), key);
        const auto res = size_type(std::distance(p.first, p.second));
        assert(res <= 1);
        _vec.erase(p.first, p.second);
        return res;
    }

    template <typename Predicate>
    auto erase_if(const Predicate& predicate) -> size_type {
        return std::erase_if(_vec, predicate);
    }

private:
    template <typename K>
    auto _find_insert_pos(const K& k) noexcept {
        const auto b = _vec.begin();
        const auto e = _vec.end();
        const auto p = _ops().lower_bound(b, e, k);

        return std::pair{p, (p == e) or not are_equal(k, p->first)};
    }

    template <typename I, typename K>
    auto _find_insert_pos(I p, const K& k) {
        auto b = _vec.begin();
        auto e = _vec.end();
        if(p == e) {
            if(_vec.empty() or value_comp()(_vec.back(), k)) {
                return std::pair{p, true};
            }
            p = _ops().lower_bound(b, e, k);
        }
        if(k == p->first) {
            return std::pair{p, false};
        }
        if(key_comp()(k, p->first)) {
            if(p != b) {
                p = _ops().lower_bound(b, p, k);
            }
        } else {
            p = _ops().lower_bound(p, e, k);
        }
        return std::pair{p, true};
    }

    template <typename I, typename K, typename... Args>
    auto _do_emplace(I ip, const K& key, Args&&... args) -> I {
        if(ip.second) {
            ip.first =
              _vec.emplace(ip.first, key, Val{std::forward<Args>(args)...});
        }
        return ip;
    }

    template <typename I, typename V>
    auto _do_insert(I ip, const V& value) -> I {
        if(ip.second) {
            ip.first = _vec.insert(ip.first, value);
        }
        return ip;
    }
};
//------------------------------------------------------------------------------
} // namespace eagine
