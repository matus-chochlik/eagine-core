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

import std;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Continuous pre-allocated container with specified maximum number of elements.
/// @ingroup container
/// @see small_vector
export template <typename T, std::size_t N>
class static_vector {
public:
    /// @brief Stored value type.
    using value_type = std::remove_const_t<T>;

    /// @brief Pointer to element type.
    using pointer = T*;

    /// @brief Pointer to const element type.
    using const_pointer = const value_type*;

    /// @brief Size type.
    using size_type = std::size_t;

    /// @brief Difference type.
    using difference_type = std::ptrdiff_t;

    /// @brief Iterator type.
    using iterator = typename std::array<T, N>::iterator;

    /// @brief Const iterator type.
    using const_iterator = typename std::array<T, N>::const_iterator;

    using allocator_type = std::allocator<T>;

    /// @brief Default constructor.
    constexpr static_vector() noexcept = default;

    /// @brief Construction with specified number of elements.
    constexpr static_vector(size_type n) noexcept {
        resize(n);
    }

    /// @brief Returns the maximum number of elements that can be stored.
    /// @see size
    /// @see full
    [[nodiscard]] constexpr auto max_size() const noexcept -> size_type {
        return N;
    }

    /// @brief Returns the current capacity of this static_vector.
    /// @see size
    /// @see resize
    [[nodiscard]] constexpr auto capacity() const noexcept -> size_type {
        return N;
    }

    /// @brief Reserves storage for the specified number of elements.
    /// @pre n <= max_size()
    /// @see size
    constexpr void reserve([[maybe_unused]] size_type n) const noexcept {
        assert(n <= max_size());
    }

    /// @brief Returns the current number of elements in this static_vector.
    /// @see max_size
    /// @see resize
    /// @see empty
    /// @see full
    [[nodiscard]] constexpr auto size() const noexcept -> std::size_t {
        return _size;
    }

    /// @brief Returns the number of available free elements in this static_vector.
    /// @see max_size
    /// @see size
    /// @see empty
    /// @see full
    [[nodiscard]] constexpr auto available() const noexcept -> std::size_t {
        return N - _size;
    }

    /// @brief Changes the current size to the specified value.
    /// @pre n <= max_size()
    /// @see max_size
    /// @see size
    /// @see capacity
    constexpr void resize(size_type n) noexcept(
      std::is_nothrow_default_constructible_v<T> &&
      std::is_nothrow_destructible_v<T>) {
        assert(n <= max_size());
        if(n < _size) {
            std::fill(_array.begin() + n, _array.begin() + _size, T{});
        }
        _size = n;
    }

    /// @brief Indicates if this static_vector is empty.
    /// @see size
    /// @see full
    [[nodiscard]] constexpr auto empty() const noexcept -> bool {
        return size() == 0;
    }

    /// @brief Indicates if this static_vector is full.
    /// @see max_size
    /// @see size
    /// @see empty
    [[nodiscard]] constexpr auto full() const noexcept -> bool {
        return size() == max_size();
    }

    /// @brief Clears all elements.
    /// @post empty()
    /// @see empty
    constexpr void clear() noexcept {
        std::fill(begin(), end(), T{});
        _size = 0;
    }

    /// @brief Returns a pointer to the beginning of the contiguous storage.
    /// @see size
    /// @see begin
    [[nodiscard]] constexpr auto data() noexcept -> pointer {
        return _array.data();
    }

    /// @brief Returns a pointer to the beginning of the contiguous storage.
    /// @see size
    /// @see begin
    [[nodiscard]] constexpr auto data() const noexcept -> const_pointer {
        return _array.data();
    }

    /// @brief Returns an iterator pointing to the first element.
    /// @see end
    [[nodiscard]] constexpr auto begin() noexcept -> iterator {
        return _array.begin();
    }

    /// @brief Returns a const iterator pointing to the first element.
    /// @see end
    [[nodiscard]] constexpr auto begin() const noexcept -> const_iterator {
        return _array.begin();
    }

    /// @brief Returns a const iterator pointing to the first element.
    /// @see end
    [[nodiscard]] constexpr auto cbegin() const noexcept -> const_iterator {
        return _array.cbegin();
    }

    /// @brief Returns an iterator pointing past the last element.
    /// @see begin
    [[nodiscard]] constexpr auto end() noexcept -> iterator {
        return _array.begin() + size();
    }

    /// @brief Returns a const iterator pointing past the last element.
    /// @see begin
    [[nodiscard]] constexpr auto end() const noexcept -> const_iterator {
        return _array.begin() + size();
    }

    /// @brief Returns a const iterator pointing past the last element.
    /// @see begin
    [[nodiscard]] constexpr auto cend() const noexcept -> const_iterator {
        return _array.cbegin() + size();
    }

    /// @brief Pushes a new element to the back of this static_vector.
    /// @pre not full()
    /// @see emplace_back
    /// @see pop_back
    /// @see insert
    constexpr void push_back(const value_type& value) noexcept(
      std::is_nothrow_copy_constructible_v<T>) {
        assert(not full());
        *end() = value;
        ++_size;
    }

    /// @brief Constructs a new element in-place at the back of this static_vector.
    /// @pre not full()
    /// @see push_back
    /// @see pop_back
    /// @see emplace
    template <typename... Args>
    constexpr void emplace_back(Args&&... args) noexcept(
      std::is_nothrow_constructible_v<T, decltype(std::forward<Args>(args))...>) {
        assert(not full());
        *end() = T(std::forward<Args>(args)...);
        ++_size;
    }

    /// @brief Removes a single element from the back of this static_vector.
    /// @pre not empty()
    /// @see push_back
    /// @see emplace_back
    /// @see erase
    /// @see clear
    constexpr void pop_back() noexcept(std::is_nothrow_destructible_v<T>) {
        assert(not empty());
        --_size;
        *end() = T{};
    }

    /// @brief Stores elements from the specified range starting at given position.
    /// @see push_back
    /// @see emplace
    template <typename Iter>
    constexpr auto insert(const_iterator pos, Iter iter, Iter iend) noexcept(
      std::is_nothrow_copy_constructible_v<T>) -> iterator {
        const auto count{std::distance(iter, iend)};
        assert((size() + count) <= max_size());
        const auto bpos{begin() + std::distance(cbegin(), pos)};
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

    /// @brief Stores the specified value at given position.
    /// @see push_back
    /// @see emplace
    constexpr auto insert(const_iterator pos, const value_type& value) noexcept(
      std::is_nothrow_copy_constructible_v<T>) -> iterator {
        assert(not full() and _valid_pos(pos));
        auto ipos{begin() + std::distance(cbegin(), pos)};
        std::move_backward(ipos, end(), end() + 1);
        *ipos = value;
        ++_size;
        return ipos;
    }

    /// @brief Stores the specified value at given position.
    /// @see push_back
    /// @see emplace
    constexpr auto insert(const_iterator pos, value_type&& value) noexcept(
      std::is_nothrow_copy_constructible_v<T>) -> iterator {
        assert(not full() and _valid_pos(pos));
        auto ipos{begin() + std::distance(cbegin(), pos)};
        std::move_backward(ipos, end(), end() + 1);
        *ipos = std::move(value);
        ++_size;
        return ipos;
    }

    /// @brief Constructs a new value in-place at the specified position from args.
    /// @see push_back
    /// @see insert
    template <typename... Args>
    constexpr auto emplace(const_iterator pos, Args&&... args) noexcept(
      std::is_nothrow_constructible_v<T, decltype(std::forward<Args>(args))...>)
      -> iterator {
        assert(not full() and _valid_pos(pos));
        auto ipos{begin() + std::distance(cbegin(), pos)};
        std::move_backward(ipos, end(), end() + 1);
        *ipos = T(std::forward<Args>(args)...);
        ++_size;
        return ipos;
    }

    /// @brief Erases the elements in the specified range.
    /// @see clear
    /// @see pop_back
    constexpr auto erase(const_iterator iter, const_iterator iend) noexcept
      -> iterator {
        assert(_valid_pos(iter) and _valid_pos(iend));
        const auto count{std::distance(iter, iend)};
        const auto epos{begin() + std::distance(cbegin(), iter)};
        const auto eend{begin() + std::distance(cbegin(), iend)};
        std::generate(epos, eend, [] { return T{}; });
        std::move(epos + count, end(), epos);
        _size -= count;
        return epos;
    }

    /// @brief Erases the elements at the specified position.
    /// @see clear
    /// @see pop_back
    constexpr auto erase(const_iterator pos) noexcept -> iterator {
        assert(_valid_pos(pos));
        auto epos{begin() + std::distance(cbegin(), pos)};
        *epos = T{};
        std::move(epos + 1, end(), epos);
        --_size;
        return epos;
    }

    /// @brief Returns reference to the first element.
    /// @pre not empty()
    /// @see back
    [[nodiscard]] constexpr auto front() noexcept -> value_type& {
        assert(not empty());
        return _array.front();
    }

    /// @brief Returns const reference to the first element.
    /// @pre not empty()
    /// @see back
    [[nodiscard]] constexpr auto front() const noexcept -> const value_type& {
        assert(not empty());
        return _array.front();
    }

    /// @brief Returns reference to the last element.
    /// @pre not empty()
    /// @see front
    [[nodiscard]] constexpr auto back() noexcept -> value_type& {
        assert(not empty());
        return _array[_size - 1];
    }

    /// @brief Returns const reference to the last element.
    /// @pre not empty()
    /// @see front
    [[nodiscard]] constexpr auto back() const noexcept -> const value_type& {
        assert(not empty());
        return _array[_size - 1];
    }

    /// @brief Returns reference to the element at the specified index.
    /// @pre not empty()
    /// @see front
    /// @see back
    [[nodiscard]] constexpr auto operator[](size_type i) noexcept
      -> value_type& {
        assert(i < size());
        return _array[i];
    }

    /// @brief Returns const reference to the element at the specified index.
    /// @pre not empty()
    /// @see front
    /// @see back
    [[nodiscard]] constexpr auto operator[](size_type i) const noexcept
      -> const value_type& {
        assert(i < size());
        return _array[i];
    }

private:
    constexpr auto _valid_pos(const_iterator pos) const noexcept -> bool {
        return (begin() <= pos) and (pos <= end());
    }

    std::array<T, N> _array{};
    std::size_t _size{0};
};
//------------------------------------------------------------------------------
/// @brief Erases elementes satisfying the specified predicate from a static_vector.
/// @ingroup container
/// @relates static_vector
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
