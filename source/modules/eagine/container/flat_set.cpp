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
        _vec = Container(il);
        std::sort(_vec.begin(), _vec.end(), value_comp());
    }

    void assign(const Container& v) {
        _vec = Container(v.begin(), v.end());
        std::sort(_vec.begin(), _vec.end(), value_comp());
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
        return _vec.empty();
    }

    /// @brief Returns the number of elements in this set.
    /// @see empty
    /// @see max_size
    [[nodiscard]] auto size() const noexcept {
        return _vec.size();
    }

    /// @brief Returns the maximum number of elements that can be stored in this set.
    /// @see size
    [[nodiscard]] auto max_size() const noexcept {
        return _vec.max_size();
    }

    /// @brief Return an iterator pointing to the first element.
    /// @see end
    /// @see find
    [[nodiscard]] auto begin() -> iterator {
        return _vec.begin();
    }

    /// @brief Return a const iterator pointing to the first element.
    /// @see end
    /// @see find
    [[nodiscard]] auto begin() const -> const_iterator {
        return _vec.begin();
    }

    /// @brief Return an iterator pointing to the last element.
    /// @see begin
    /// @see find
    [[nodiscard]] auto end() -> iterator {
        return _vec.end();
    }

    /// @brief Return a const iterator pointing to the last element.
    /// @see begin
    /// @see find
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

    /// @brief Returns the iterator pointing to the element with the specified key.
    /// @see contains
    [[nodiscard]] auto find(const Key& key) const noexcept {
        auto [p, i] = _find_insert_pos(key);
        return i ? _vec.end() : p;
    }

    /// @brief Indicates if the specified key is stored in this set.
    /// @see find
    [[nodiscard]] auto contains(const Key& key) const noexcept {
        return not _find_insert_pos(key).second;
    }

    /// @brief Clears all elements from this set.
    auto clear() noexcept {
        _vec.clear();
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
        return _vec.erase(p);
    }

    /// @brief Erases the element between the specified iterators.
    auto erase(iterator f, iterator t) -> iterator {
        return _vec.erase(f, t);
    }

    /// @brief Erases the specified key from this set.
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
