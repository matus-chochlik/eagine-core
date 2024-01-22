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

import std;
import eagine.core.types;
import eagine.core.memory;
import :static_vector;
import :chunk_list;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Drop-in replacement for std::set with a contiguous node key/value.
/// @ingroup container
/// @note Unlike with std::set insertion and deletion of elements invalidates
/// existing iterators.
export template <
  typename Key,
  typename Compare = std::less<Key>,
  class Container = std::vector<Key>>
class flat_set : private Compare {

    Container _storage;

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

    /// @brief Default constructor.
    flat_set() noexcept = default;

    /// @brief Copy constructor.
    flat_set(const flat_set&) = default;

    /// @brief Move constructor.
    flat_set(flat_set&&) noexcept = default;

    /// @brief Copy assignment.
    auto operator=(const flat_set&) -> flat_set& = default;

    /// @brief Move assignment.
    auto operator=(flat_set&&) noexcept -> flat_set& = default;

    ~flat_set() noexcept = default;

    /// @brief Construction from an initializer list of elements.
    flat_set(std::initializer_list<Key> il) {
        assign(il);
    }

    flat_set(const Container& v) {
        assign(v);
    }

    /// @brief Replaces the content with elements from an initializer list.
    void assign(std::initializer_list<Key> il) {
        using std::sort;
        _storage = Container(il);
        sort(_storage.begin(), _storage.end(), value_comp());
    }

    void assign(const Container& v) {
        using std::sort;
        _storage = Container(v.begin(), v.end());
        sort(_storage.begin(), _storage.end(), value_comp());
    }

    /// @brief Returns the key comparator.
    [[nodiscard]] auto key_comp() const noexcept -> const Compare& {
        return *this;
    }

    /// @brief Returns the key comparator.
    [[nodiscard]] auto value_comp() const noexcept -> const Compare& {
        return *this;
    }

    /// @brief Indicates if this set is empty.
    /// @see size
    [[nodiscard]] auto empty() const noexcept {
        return _storage.empty();
    }

    /// @brief Returns the number of elements in this set.
    /// @see empty
    /// @see max_size
    [[nodiscard]] auto size() const noexcept {
        return _storage.size();
    }

    /// @brief Returns the maximum number of elements that can be stored in this set.
    /// @see size
    [[nodiscard]] auto max_size() const noexcept {
        return _storage.max_size();
    }

    /// @brief Return an iterator pointing to the first element.
    /// @see end
    /// @see find
    [[nodiscard]] auto begin() -> iterator {
        return _storage.begin();
    }

    /// @brief Return a const iterator pointing to the first element.
    /// @see end
    /// @see find
    [[nodiscard]] auto begin() const -> const_iterator {
        return _storage.begin();
    }

    /// @brief Return an iterator pointing to the last element.
    /// @see begin
    /// @see find
    [[nodiscard]] auto end() -> iterator {
        return _storage.end();
    }

    /// @brief Return a const iterator pointing to the last element.
    /// @see begin
    /// @see find
    [[nodiscard]] auto end() const -> const_iterator {
        return _storage.end();
    }

    [[nodiscard]] auto lower_bound(const Key& key) const noexcept {
        using ::std::lower_bound;
        return lower_bound(begin(), end(), key, value_comp());
    }

    [[nodiscard]] auto upper_bound(const Key& key) const noexcept {
        using ::std::upper_bound;
        return upper_bound(begin(), end(), key, value_comp());
    }

    [[nodiscard]] auto equal_range(const Key& key) const noexcept {
        using ::std::equal_range;
        return equal_range(begin(), end(), key, value_comp());
    }

    /// @brief Returns the iterator pointing to the element with the specified key.
    /// @see contains
    [[nodiscard]] auto find(const Key& key) const noexcept {
        auto [p, i] = _find_insert_pos(key);
        return i ? _storage.end() : p;
    }

    /// @brief Indicates if the specified key is stored in this set.
    /// @see find
    [[nodiscard]] auto contains(const Key& key) const noexcept {
        return not _find_insert_pos(key).second;
    }

    /// @brief Reserves the underlying storage for the specified number of elements.
    auto reserve(const size_type sz) -> auto& {
        _storage.reserve(sz);
        return *this;
    }

    /// @brief Clears all elements from this set.
    auto clear() noexcept {
        _storage.clear();
    }

    /// @brief Inserts the specified value into this set.
    auto insert(const value_type& value) -> std::pair<iterator, bool> {
        auto ip = _find_insert_pos(value);
        ip = _do_insert(ip, value);
        return {ip.first, ip.second};
    }

    /// @brief Inserts the specified value into this set with position hint.
    auto insert(iterator p, const value_type& value) -> iterator {
        const auto ip = _find_insert_pos(p, value);
        return _do_insert(ip, value).first;
    }

    /// @brief Erases the element pointed to by the specified iterator.
    auto erase(iterator p) -> iterator {
        return _storage.erase(p);
    }

    /// @brief Erases the element between the specified iterators.
    auto erase(iterator f, iterator t) -> iterator {
        return _storage.erase(f, t);
    }

    /// @brief Erases the specified key from this set.
    auto erase(const Key& key) -> size_type {
        using std::distance;
        using std::lower_bound;
        using std::upper_bound;
        const auto p{
          lower_bound(_storage.begin(), _storage.end(), key, key_comp())};
        if(p != _storage.end() and not key_comp()(key, *p)) {
            assert(
              distance(
                p,
                upper_bound(
                  _storage.begin(), _storage.end(), key, key_comp())) == 1);
            _storage.erase(p);
            return 1;
        }
        return 0;
    }

    [[nodiscard]] auto underlying() const noexcept {
        using memory::view;
        return view(_storage);
    }

private:
    auto _find_insert_pos(const Key& k) const noexcept {
        using std::lower_bound;
        const auto b = _storage.begin();
        const auto e = _storage.end();
        const auto p = lower_bound(b, e, k, key_comp());

        return std::pair{p, (p == e) or (k != *p)};
    }

    template <typename I>
    auto _find_insert_pos(I p, const Key& k) const noexcept {
        using std::lower_bound;
        auto b = _storage.begin();
        auto e = _storage.end();
        if(p == e) {
            if(_storage.empty() or value_comp()(_storage.back(), k)) {
                return std::pair{p, true};
            }
            p = lower_bound(b, e, k, key_comp());
        }
        if(k == *p) {
            return std::pair{p, false};
        }
        if(key_comp()(k, *p)) {
            if(p != b) {
                p = lower_bound(b, p, k, key_comp());
            }
        } else {
            p = lower_bound(p, e, k, key_comp());
        }
        return std::pair{p, true};
    }

    template <typename I>
    auto _do_insert(I ip, const Key& value) -> I {
        if(ip.second) {
            ip.first = _storage.insert(ip.first, value);
        }
        return ip;
    }
};
//------------------------------------------------------------------------------
export template <
  typename Key,
  std::size_t MaxSize,
  typename Compare = std::less<Key>>
using static_flat_set = flat_set<Key, Compare, static_vector<Key, MaxSize>>;
//------------------------------------------------------------------------------
export template <
  typename Key,
  std::size_t ChunkSize,
  typename Compare = std::less<Key>>
using chunk_set = flat_set<Key, Compare, chunk_list<Key, ChunkSize>>;
//------------------------------------------------------------------------------
export template <typename W, typename T, typename C, typename S>
auto view(const flat_set<T, C, S>& c) noexcept {
    return c.underlying();
}
//------------------------------------------------------------------------------
export template <typename W, typename T, typename C, typename S>
constexpr auto find(flat_set<T, C, S>& s, W&& what) noexcept {
    return optional_iterator{s, s.find(std::forward<W>(what))};
}

export template <typename W, typename T, typename C, typename S>
constexpr auto find(const flat_set<T, C, S>& s, W&& what) noexcept {
    return optional_iterator{s, s.find(std::forward<W>(what))};
}
//------------------------------------------------------------------------------
} // namespace eagine
