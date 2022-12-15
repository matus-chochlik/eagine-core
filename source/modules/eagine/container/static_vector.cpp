/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.container:static_vector;

import <algorithm>;
export import <array>;
import <cstdint>;

namespace eagine {
//------------------------------------------------------------------------------
export template <typename T, std::size_t N>
class static_vector {
public:
    using value_type = std::remove_const_t<T>;
    using pointer = T*;
    using const_pointer = const value_type*;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using iterator = pointer;
    using const_iterator = const_pointer;

    constexpr static_vector() noexcept = default;

    [[nodiscard]] constexpr auto max_size() const noexcept -> size_type {
        return N;
    }

    [[nodiscard]] constexpr auto capacity() const noexcept -> size_type {
        return N;
    }

    constexpr void reserve([[maybe_unused]] size_type n) const noexcept {
        assert(n <= N);
    }

    [[nodiscard]] constexpr auto size() const noexcept -> std::size_t {
        return _size;
    }

    constexpr void resize(size_type n) noexcept(
      std::is_nothrow_default_constructible_v<T>&&
        std::is_nothrow_destructible_v<T>) {
        assert(n <= max_size());
        if(n < _size) {
            std::fill(_array.begin() + n, _array.begin() + _size, T{});
        }
        _size = n;
    }

    [[nodiscard]] constexpr auto empty() const noexcept -> bool {
        return size() == 0;
    }

    [[nodiscard]] constexpr auto full() const noexcept -> bool {
        return size() == max_size();
    }

    constexpr void clear() noexcept {
        std::fill(begin(), end(), T{});
        _size = 0;
    }

    [[nodiscard]] constexpr auto data() noexcept -> pointer {
        return _array.data();
    }

    [[nodiscard]] constexpr auto data() const noexcept -> const_pointer {
        return _array.data();
    }

    [[nodiscard]] constexpr auto begin() noexcept -> iterator {
        return data();
    }

    [[nodiscard]] constexpr auto begin() const noexcept -> const_iterator {
        return data();
    }

    [[nodiscard]] constexpr auto cbegin() const noexcept -> const_iterator {
        return data();
    }

    [[nodiscard]] constexpr auto end() noexcept -> iterator {
        return data() + size();
    }

    [[nodiscard]] constexpr auto end() const noexcept -> const_iterator {
        return data() + size();
    }

    [[nodiscard]] constexpr auto cend() const noexcept -> const_iterator {
        return data() + size();
    }

    constexpr void push_back(const value_type& value) noexcept(
      std::is_nothrow_copy_constructible_v<T>) {
        assert(!full());
        *end() = value;
        ++_size;
    }

    template <typename... Args>
    constexpr void emplace_back(Args&&... args) noexcept(
      std::is_nothrow_constructible_v<T, decltype(std::forward<Args>(args))...>) {
        assert(!full());
        *end() = T(std::forward<Args>(args)...);
        ++_size;
    }

    constexpr void pop_back() noexcept(std::is_nothrow_destructible_v<T>) {
        assert(!empty());
        --_size;
        *end() = T{};
    }

    template <typename Iter>
    constexpr auto insert(const_iterator pos, Iter iter, Iter iend) noexcept(
      std::is_nothrow_copy_constructible_v<T>) -> iterator {
        const auto count{std::distance(iter, iend)};
        assert((size() + count) <= max_size());
        assert(_valid_pos(iter) && _valid_pos(iend));
        const auto bpos{begin() + std::distance(cbegin(), iter)};
        auto ipos{bpos};
        std::move_backward(ipos, end(), end() + count);
        while(iter != iend) {
            *ipos = *iter;
            ++ipos;
            ++iter;
        }
        _size += count;
        return bpos;
    }

    constexpr auto insert(const_iterator pos, const value_type& value) noexcept(
      std::is_nothrow_copy_constructible_v<T>) -> iterator {
        assert(!full() && _valid_pos(pos));
        auto ipos{begin() + std::distance(cbegin(), pos)};
        std::move_backward(ipos, end(), end() + 1);
        *ipos = value;
        ++_size;
        return ipos;
    }

    constexpr auto insert(const_iterator pos, value_type&& value) noexcept(
      std::is_nothrow_copy_constructible_v<T>) -> iterator {
        assert(!full() && _valid_pos(pos));
        auto ipos{begin() + std::distance(cbegin(), pos)};
        std::move_backward(ipos, end(), end() + 1);
        *ipos = std::move(value);
        ++_size;
        return ipos;
    }

    template <typename... Args>
    constexpr auto emplace(const_iterator pos, Args&&... args) noexcept(
      std::is_nothrow_constructible_v<T, decltype(std::forward<Args>(args))...>)
      -> iterator {
        assert(!full() && _valid_pos(pos));
        auto ipos{begin() + std::distance(cbegin(), pos)};
        std::move_backward(ipos, end(), end() + 1);
        *ipos = T(std::forward<Args>(args)...);
        ++_size;
        return ipos;
    }

    constexpr auto erase(const_iterator iter, const_iterator iend) noexcept
      -> iterator {
        assert(_valid_pos(iter) && _valid_pos(iend));
        const auto count{std::distance(iter, iend)};
        const auto epos{begin() + std::distance(cbegin(), iter)};
        const auto eend{begin() + std::distance(cbegin(), iend)};
        std::fill(epos, eend, T{});
        std::move(epos + count, end(), epos);
        _size -= count;
        return epos;
    }

    constexpr auto erase(const_iterator pos) noexcept -> iterator {
        assert(_valid_pos(pos));
        auto epos{begin() + std::distance(cbegin(), pos)};
        *epos = T{};
        std::move(epos + 1, end(), epos);
        --_size;
        return epos;
    }

    [[nodiscard]] constexpr auto front() noexcept -> value_type& {
        assert(!empty());
        return _array.front();
    }

    [[nodiscard]] constexpr auto front() const noexcept -> const value_type& {
        assert(!empty());
        return _array.front();
    }

    [[nodiscard]] constexpr auto back() noexcept -> value_type& {
        assert(!empty());
        return _array.back();
    }

    [[nodiscard]] constexpr auto back() const noexcept -> const value_type& {
        assert(!empty());
        return _array.back();
    }

    [[nodiscard]] constexpr auto operator[](size_type i) noexcept
      -> value_type& {
        assert(i < size());
        return _array[i];
    }

    [[nodiscard]] constexpr auto operator[](size_type i) const noexcept
      -> const value_type& {
        assert(i < size());
        return _array[i];
    }

private:
    constexpr auto _valid_pos(const_iterator pos) const noexcept -> bool {
        return (begin() <= pos) && (pos <= end());
    }

    std::array<T, N> _array{};
    std::size_t _size{0};
};
//------------------------------------------------------------------------------
export template <typename T, std::size_t N, typename Predicate>
auto erase_if(static_vector<T, N>& v, const Predicate& predicate) noexcept ->
  typename static_vector<T, N>::size_type {
    const auto pos{std::remove_if(v.begin(), v.end(), predicate)};
    const auto num{std::distance(pos, v.end())};
    v.erase(pos, v.end());
    return num;
}
//------------------------------------------------------------------------------
} // namespace eagine
