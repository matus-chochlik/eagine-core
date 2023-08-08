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

import std;
import eagine.core.types;
import eagine.core.memory;

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
/// @brief Base class for flat_map.
/// @ingroup container
/// @note Do not use directly, use flat_map instead.
export template <typename Key, typename Val, typename Cmp, typename Derived>
class flat_map_view_crtp : flat_map_ops<Key, Val, Cmp> {
    using _ops_t = flat_map_ops<Key, Val, Cmp>;

public:
    /// @brief The key type.
    using key_type = Key;

    /// @brief The mapped value type.
    using mapped_type = Val;

    /// @brief The key/value pair type.
    using value_type = std::pair<const Key, Val>;

    /// @brief Reference type.
    using reference = value_type&;

    /// @brief Const reference type.
    using const_reference = const value_type&;

    /// @brief Key comparator type.
    using key_compare = Cmp;
    using value_compare = flat_map_value_compare<Key, Val, Cmp>;

    /// @brief Returns a reference to the key comparator object.
    [[nodiscard]] auto key_comp() const noexcept -> const key_compare& {
        return _ops().value_comp().key_comp();
    }

    [[nodiscard]] auto value_comp() const noexcept -> const value_compare& {
        return _ops().value_comp();
    }

    /// @brief Indicates if this map is empty.
    /// @see size
    [[nodiscard]] auto empty() const noexcept -> bool {
        return _ops().empty(_b(), _e());
    }

    /// @brief Returns the number of elements in this map.
    /// @see empty
    [[nodiscard]] auto size() const noexcept {
        return _ops().size(_b(), _e());
    }

    /// @brief Returns the iterator pointing to the specified key.
    /// @see contains
    template <typename K>
    [[nodiscard]] auto find(const K& key) noexcept {
        return _ops().find(_b(), _e(), key);
    }

    /// @brief Returns the iterator pointing to the specified key.
    /// @see contains
    template <typename K>
    [[nodiscard]] auto find(const K& key) const noexcept {
        return _ops().find(_b(), _e(), key);
    }

    /// @brief Indicates if this map contains the specified key.
    /// @see find
    template <typename K>
    [[nodiscard]] auto contains(const K& key) const noexcept -> bool {
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

    /// @brief Returns the value stored under the specified key.
    template <typename K>
    [[nodiscard]] auto at(const K& key) -> Val& {
        return _ops().at(_b(), _e(), key);
    }

    /// @brief Returns the value stored under the specified key.
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
/// @brief Drop-in replacement for std::map with a contiguous node key/value.
/// @ingroup container
/// @note Unlike with std::map insertion and deletion of elements invalidates
/// existing iterators.
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

    /// @brief Default constructor.
    flat_map() noexcept = default;

    /// @brief Copy constructor.
    flat_map(const flat_map&) = default;

    /// @brief Move constructor.
    flat_map(flat_map&&) noexcept = default;

    /// @brief Copy assignment.
    auto operator=(const flat_map&) -> flat_map& = default;

    /// @brief Move assignment.
    auto operator=(flat_map&&) noexcept -> flat_map& = default;
    ~flat_map() noexcept = default;

    /// @brief Construction from an initializer list of key/value pairs.
    flat_map(std::initializer_list<std::pair<Key, Val>> il) {
        assign(il);
    }

    flat_map(const std::vector<value_type>& v) {
        assign(v);
    }

    /// @brief Replaces the elements with keys/values from an initializer list.
    void assign(std::initializer_list<std::pair<Key, Val>> il) {
        _vec.assign(il.begin(), il.end());
        std::sort(_vec.begin(), _vec.end(), value_comp());
    }

    void assign(const std::vector<value_type>& v) {
        _vec.assign(v.begin(), v.end());
        std::sort(_vec.begin(), _vec.end(), value_comp());
    }

    /// @brief Indicates if this map is empty.
    /// @see size
    /// @see clear
    [[nodiscard]] auto empty() const noexcept -> bool {
        return _vec.empty();
    }

    /// @brief Returns the maximum number of elements in this map.
    [[nodiscard]] auto max_size() const noexcept -> size_type {
        return _vec.max_size();
    }

    /// @brief Reserves the underlying storage for the specified number of elements.
    auto reserve(const size_type sz) -> auto& {
        _vec.reserve(sz);
        return *this;
    }

    /// @brief Clears all elements from this map.
    /// @see empty
    auto clear() -> auto& {
        _vec.clear();
        return *this;
    }

    /// @brief Returns the element with the lowest key value.
    /// @pre not empty()
    /// @see back
    /// @see begin
    [[nodiscard]] auto front() const noexcept -> auto& {
        return _vec.front();
    }

    /// @brief Returns the element with the highest key value.
    /// @pre not empty()
    /// @see front
    /// @see end
    [[nodiscard]] auto back() const noexcept -> auto& {
        return _vec.back();
    }

    /// @brief Returns an iterator pointing to the first element.
    /// @see end
    /// @see front
    [[nodiscard]] auto begin() noexcept -> iterator {
        return _vec.begin();
    }

    /// @brief Returns a const iterator pointing to the first element.
    /// @see end
    /// @see front
    [[nodiscard]] auto begin() const -> const_iterator {
        return _vec.begin();
    }

    /// @brief Returns an iterator pointing past the last element.
    /// @see begin
    /// @see back
    [[nodiscard]] auto end() -> iterator {
        return _vec.end();
    }

    /// @brief Returns a const iterator pointing past the last element.
    /// @see begin
    /// @see back
    [[nodiscard]] auto end() const -> const_iterator {
        return _vec.end();
    }

    /// @brief Returns a reference to the element with the specified key.
    [[nodiscard]] auto operator[](const Key& key) -> auto& {
        auto ip = _find_insert_pos(key);
        ip = _do_emplace(ip, key);
        return ip.first->second;
    }

    /// @brief Constructs a new element with the specified arguments under specified key.
    /// @see insert
    template <typename... Args>
    auto emplace(const Key& key, Args&&... args) -> std::pair<iterator, bool> {
        auto ip = _find_insert_pos(key);
        ip = _do_emplace(ip, key, std::forward<Args>(args)...);
        return ip;
    }

    /// @brief Constructs a new element with the specified arguments under specified key.
    /// @see insert
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

    /// @brief Inserts a new key/value pair.
    /// @see emplace
    auto insert(const value_type& value) -> std::pair<iterator, bool> {
        auto ip = _find_insert_pos(value.first);
        ip = _do_insert(ip, value);
        return {ip.first, ip.second};
    }

    /// @brief Inserts a new key/value pair with the specified hint.
    /// @see emplace
    auto insert(iterator p, const value_type& value) -> iterator {
        const auto ip = _find_insert_pos(p, value.first);
        return _do_insert(ip, value).first;
    }

    /// @brief Erases the element pointed to by the specified iterator.
    /// @see erase_if
    auto erase(iterator p) -> iterator {
        return _vec.erase(p);
    }

    /// @brief Erases elements between the given pair of iterators.
    /// @see erase_if
    auto erase(iterator f, iterator t) -> iterator {
        return _vec.erase(f, t);
    }

    /// @brief Erases the element stored under the specified key.
    /// @see erase_if
    template <typename K>
    auto erase(const K& key) -> size_type {
        const auto p = _ops().equal_range(_vec.begin(), _vec.end(), key);
        const auto res = size_type(std::distance(p.first, p.second));
        assert(res <= 1);
        _vec.erase(p.first, p.second);
        return res;
    }

    /// @brief Erases all elements satisfying the specified predicate.
    /// @see erase
    template <typename Predicate>
    auto erase_if(const Predicate& predicate) -> size_type {
        return std::erase_if(_vec, predicate);
    }

    auto _vec_ref() const noexcept -> const auto& {
        return _vec;
    }

    auto _vec_ref() noexcept -> auto& {
        return _vec;
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
export template <typename Key, typename Val, typename Cmp, typename Container>
auto view(const flat_map<Key, Val, Cmp, Container>& c) noexcept {
    using memory::view;
    return view(c._vec_ref());
}

export template <typename Key, typename Val, typename Cmp, typename Container>
auto cover(flat_map<Key, Val, Cmp, Container>& c) noexcept {
    using memory::cover;
    return cover(c._vec_ref());
}
//------------------------------------------------------------------------------
export template <typename W, typename K, typename T, typename C, typename A>
constexpr auto find(flat_map<K, T, C, A>& m, W&& what) noexcept
  -> optional_reference<T> {
    if(const auto pos{m.find(std::forward<W>(what))}; pos != m.end()) {
        return {std::get<1>(*pos)};
    }
    return {};
}

export template <typename W, typename K, typename T, typename C, typename A>
constexpr auto find(const flat_map<K, T, C, A>& m, W&& what) noexcept
  -> optional_reference<const T> {
    if(const auto pos{m.find(std::forward<W>(what))}; pos != m.end()) {
        return {std::get<1>(*pos)};
    }
    return {};
}
//------------------------------------------------------------------------------
} // namespace eagine
