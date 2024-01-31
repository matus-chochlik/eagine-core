/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.container:small_vector;

import std;
import eagine.core.types;
import :static_vector;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Replacement for std::vector with storage for small number of elements.
/// @ingroup container
/// @see static_vector
export template <typename T, std::size_t N, typename Allocator = std::allocator<T>>
class small_vector {
    using T0 = static_vector<T, N>;
    using T1 = std::vector<T, Allocator>;

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
    using iterator = pointer;

    /// @brief Const iterator type.
    using const_iterator = const_pointer;

    /// @brief Allocator type.
    using allocator_type = std::allocator<T>;

    /// @brief Default constructor.
    constexpr small_vector() noexcept = default;

    /// @brief Returns the maximum number of elements that can be stored.
    /// @see size
    /// @see full
    [[nodiscard]] constexpr auto max_size() const noexcept -> size_type {
        if(std::holds_alternative<T0>(_storage)) {
            return std::get<T0>(_storage).max_size();
        } else {
            return std::get<T1>(_storage).max_size();
        }
    }

    /// @brief Returns the current capacity of this static_vector.
    /// @see size
    /// @see resize
    [[nodiscard]] constexpr auto capacity() const noexcept -> size_type {
        if(std::holds_alternative<T0>(_storage)) {
            return std::get<T0>(_storage).size();
        } else {
            return std::get<T1>(_storage).size();
        }
    }

    /// @brief Reserves storage for the specified number of elements.
    /// @see size
    constexpr void reserve(size_type n) const noexcept {
        if(std::holds_alternative<T0>(_storage)) {
            std::get<T0>(_storage).reserve(n);
        } else {
            std::get<T1>(_storage).reserve(n);
        }
    }

    /// @brief Returns the current number of elements in this static_vector.
    /// @see max_size
    /// @see resize
    /// @see empty
    /// @see full
    [[nodiscard]] constexpr auto size() const noexcept -> size_type {
        if(std::holds_alternative<T0>(_storage)) {
            return std::get<T0>(_storage).size();
        } else {
            return std::get<T1>(_storage).size();
        }
    }

    /// @brief Changes the current size to the specified value.
    /// @pre n <= max_size()
    /// @see max_size
    /// @see size
    /// @see capacity
    constexpr void resize(size_type n) noexcept {
        if(std::holds_alternative<T0>(_storage)) {
            auto& v0{std::get<T0>(_storage)};
            if(n <= v0.max_size()) {
                v0.resize(n);
            } else {
                _storage = T1(v0.begin(), v0.end());
                auto& v1{std::get<T1>(_storage)};
                v1.resize(n);
            }
        } else {
            std::get<T1>(_storage).resize(n);
        }
    }

    /// @brief Indicates if this static_vector is empty.
    /// @see size
    /// @see full
    [[nodiscard]] constexpr auto empty() const noexcept -> bool {
        if(std::holds_alternative<T0>(_storage)) {
            return std::get<T0>(_storage).empty();
        } else {
            return std::get<T1>(_storage).empty();
        }
    }

    /// @brief Clears all elements.
    /// @post empty()
    /// @see empty
    constexpr void clear() noexcept {
        if(std::holds_alternative<T0>(_storage)) {
            std::get<T0>(_storage).clear();
        } else {
            std::get<T1>(_storage).clear();
        }
    }

    /// @brief Returns a pointer to the beginning of the contiguous storage.
    /// @see size
    /// @see begin
    [[nodiscard]] constexpr auto data() noexcept -> pointer {
        if(std::holds_alternative<T0>(_storage)) {
            return std::get<T0>(_storage).data();
        } else {
            return std::get<T1>(_storage).data();
        }
    }

    /// @brief Returns a pointer to the beginning of the contiguous storage.
    /// @see size
    /// @see begin
    [[nodiscard]] constexpr auto data() const noexcept -> const_pointer {
        if(std::holds_alternative<T0>(_storage)) {
            return std::get<T0>(_storage).data();
        } else {
            return std::get<T1>(_storage).data();
        }
    }

    /// @brief Returns an iterator pointing to the first element.
    /// @see end
    [[nodiscard]] constexpr auto begin() noexcept -> iterator {
        return data();
    }

    /// @brief Returns a const iterator pointing to the first element.
    /// @see end
    [[nodiscard]] constexpr auto begin() const noexcept -> const_iterator {
        return data();
    }

    /// @brief Returns a const iterator pointing to the first element.
    /// @see end
    [[nodiscard]] constexpr auto cbegin() const noexcept -> const_iterator {
        return data();
    }

    /// @brief Returns an iterator pointing past the last element.
    /// @see begin
    [[nodiscard]] constexpr auto end() noexcept -> iterator {
        return data() + size();
    }

    /// @brief Returns a const iterator pointing past the last element.
    /// @see begin
    [[nodiscard]] constexpr auto end() const noexcept -> const_iterator {
        return data() + size();
    }

    /// @brief Returns a const iterator pointing past the last element.
    /// @see begin
    [[nodiscard]] constexpr auto cend() const noexcept -> const_iterator {
        return data() + size();
    }

    /// @brief Pushes a new element to the back of this static_vector.
    /// @see emplace_back
    /// @see pop_back
    /// @see insert
    constexpr void push_back(const value_type& value) noexcept {
        if(std::holds_alternative<T0>(_storage)) {
            auto& v0{std::get<T0>(_storage)};
            if(v0.full()) {
                _storage = T1(v0.begin(), v0.end());
                auto& v1{std::get<T1>(_storage)};
                v1.push_back(value);
            } else {
                v0.push_back(value);
            }
        } else {
            std::get<T1>(_storage).push_back(value);
        }
    }

    /// @brief Constructs a new element in-place at the back of this static_vector.
    /// @pre not full()
    /// @see push_back
    /// @see pop_back
    /// @see emplace
    template <typename... Args>
    constexpr void emplace_back(Args&&... args) noexcept {
        if(std::holds_alternative<T0>(_storage)) {
            auto& v0{std::get<T0>(_storage)};
            if(v0.full()) {
                _storage = T1(v0.begin(), v0.end());
                auto& v1{std::get<T1>(_storage)};
                v1.emplace_back(std::forward<Args>(args)...);
            } else {
                v0.emplace_back(std::forward<Args>(args)...);
            }
        } else {
            std::get<T1>(_storage).emplace_back(std::forward<Args>(args)...);
        }
    }

    /// @brief Removes a single element from the back of this static_vector.
    /// @pre not empty()
    /// @see push_back
    /// @see emplace_back
    /// @see erase
    /// @see clear
    constexpr void pop_back() noexcept {
        if(std::holds_alternative<T0>(_storage)) {
            std::get<T0>(_storage).pop_back();
        } else {
            std::get<T1>(_storage).pop_back();
        }
    }

    /// @brief Stores elements from the specified range starting at given position.
    /// @see push_back
    /// @see emplace
    template <typename Iter>
    constexpr auto insert(const_iterator pos, Iter iter, Iter end) -> iterator {
        const auto offs{std::distance(cbegin(), pos)};
        const auto count{std::distance(iter, end)};
        if(std::holds_alternative<T0>(_storage)) {
            auto& v0{std::get<T0>(_storage)};
            if((v0.size() + count) > v0.max_size()) {
                _storage = T1(v0.begin(), v0.end());
                auto& v1{std::get<T1>(_storage)};
                v1.insert(v1.begin() + offs, iter, end);
            } else {
                v0.insert(pos, iter, end);
            }
        } else {
            auto& v1{std::get<T1>(_storage)};
            v1.insert(v1.begin() + offs, iter, end);
        }
        return begin() + offs;
    }

    /// @brief Stores the specified value at given position.
    /// @see push_back
    /// @see emplace
    constexpr auto insert(const_iterator pos, const value_type& value)
      -> iterator {
        const auto offs{std::distance(cbegin(), pos)};
        if(std::holds_alternative<T0>(_storage)) {
            auto& v0{std::get<T0>(_storage)};
            if(v0.full()) {
                _storage = T1(v0.begin(), v0.end());
                auto& v1{std::get<T1>(_storage)};
                v1.insert(v1.begin() + offs, value);
            } else {
                v0.insert(pos, value);
            }
        } else {
            auto& v1{std::get<T1>(_storage)};
            v1.insert(v1.begin() + offs, value);
        }
        return begin() + offs;
    }

    /// @brief Stores the specified value at given position.
    /// @see push_back
    /// @see emplace
    constexpr auto insert(const_iterator pos, value_type&& value) -> iterator {
        const auto offs{std::distance(cbegin(), pos)};
        if(std::holds_alternative<T0>(_storage)) {
            auto& v0{std::get<T0>(_storage)};
            if(v0.full()) {
                _storage = T1(v0.begin(), v0.end());
                auto& v1{std::get<T1>(_storage)};
                v1.insert(v1.begin() + offs, std::move(value));
            } else {
                v0.insert(pos, std::move(value));
            }
        } else {
            auto& v1{std::get<T1>(_storage)};
            v1.insert(v1.begin() + offs, std::move(value));
        }
        return begin() + offs;
    }

    /// @brief Constructs a new value in-place at the specified position from args.
    /// @see push_back
    /// @see insert
    template <typename... Args>
    constexpr auto emplace(const_iterator pos, Args&&... args) -> iterator {
        const auto offs{std::distance(cbegin(), pos)};
        if(std::holds_alternative<T0>(_storage)) {
            auto& v0{std::get<T0>(_storage)};
            if(v0.full()) {
                T1 temp{};
                temp.reserve(v0.size());
                for(auto& e : v0) {
                    temp.emplace_back(std::move(e));
                }
                _storage = std::move(temp);
                auto& v1{std::get<T1>(_storage)};
                v1.emplace(v1.begin() + offs, std::forward<Args>(args)...);
            } else {
                v0.emplace(pos, std::forward<Args>(args)...);
            }
        } else {
            auto& v1{std::get<T1>(_storage)};
            v1.emplace(v1.begin() + offs, std::forward<Args>(args)...);
        }
        return begin() + offs;
    }

    /// @brief Erases the elements in the specified range.
    /// @see clear
    /// @see pop_back
    constexpr auto erase(const_iterator iter, const_iterator end) noexcept
      -> iterator {
        const auto offs{std::distance(cbegin(), iter)};
        const auto effs{std::distance(cbegin(), end)};
        if(std::holds_alternative<T0>(_storage)) {
            auto& v0{std::get<T0>(_storage)};
            v0.erase(v0.begin() + offs, v0.begin() + effs);
        } else {
            auto& v1{std::get<T1>(_storage)};
            v1.erase(v1.begin() + offs, v1.begin() + effs);
        }
        return begin() + offs;
    }

    /// @brief Erases the elements at the specified position.
    /// @see clear
    /// @see pop_back
    constexpr auto erase(const_iterator pos) noexcept -> iterator {
        const auto offs{std::distance(cbegin(), pos)};
        if(std::holds_alternative<T0>(_storage)) {
            auto& v0{std::get<T0>(_storage)};
            v0.erase(v0.begin() + offs);
        } else {
            auto& v1{std::get<T1>(_storage)};
            v1.erase(v1.begin() + offs);
        }
        return begin() + offs;
    }

    template <typename Predicate>
    constexpr auto erase_if(const Predicate& predicate) noexcept -> size_type {
        if(std::holds_alternative<T0>(_storage)) {
            return eagine::erase_if(std::get<T0>(_storage), predicate);
        } else {
            return std::erase_if(std::get<T1>(_storage), predicate);
        }
    }

    /// @brief Returns reference to the first element.
    /// @pre not empty()
    /// @see back
    [[nodiscard]] constexpr auto front() noexcept -> value_type& {
        if(std::holds_alternative<T0>(_storage)) {
            return std::get<T0>(_storage).front();
        } else {
            return std::get<T1>(_storage).front();
        }
    }

    /// @brief Returns const reference to the first element.
    /// @pre not empty()
    /// @see back
    [[nodiscard]] constexpr auto front() const noexcept -> const value_type& {
        if(std::holds_alternative<T0>(_storage)) {
            return std::get<T0>(_storage).front();
        } else {
            return std::get<T1>(_storage).front();
        }
    }

    /// @brief Returns reference to the last element.
    /// @pre not empty()
    /// @see front
    [[nodiscard]] constexpr auto back() noexcept -> value_type& {
        if(std::holds_alternative<T0>(_storage)) {
            return std::get<T0>(_storage).back();
        } else {
            return std::get<T1>(_storage).back();
        }
    }

    /// @brief Returns const reference to the last element.
    /// @pre not empty()
    /// @see front
    [[nodiscard]] constexpr auto back() const noexcept -> const value_type& {
        if(std::holds_alternative<T0>(_storage)) {
            return std::get<T0>(_storage).back();
        } else {
            return std::get<T1>(_storage).back();
        }
    }

    /// @brief Returns reference to the element at the specified index.
    /// @pre not empty()
    /// @see front
    /// @see back
    [[nodiscard]] constexpr auto operator[](size_type i) noexcept
      -> value_type& {
        if(std::holds_alternative<T0>(_storage)) {
            return std::get<T0>(_storage)[i];
        } else {
            return std::get<T1>(_storage)[i];
        }
    }

    /// @brief Returns const reference to the element at the specified index.
    /// @pre not empty()
    /// @see front
    /// @see back
    [[nodiscard]] constexpr auto operator[](size_type i) const noexcept
      -> const value_type& {
        if(std::holds_alternative<T0>(_storage)) {
            return std::get<T0>(_storage)[i];
        } else {
            return std::get<T1>(_storage)[i];
        }
    }

private:
    std::variant<T0, T1> _storage{};
};
//------------------------------------------------------------------------------
export template <typename W, typename T, std::size_t N, typename A>
constexpr auto find(small_vector<T, N, A>& v, W&& what) noexcept {
    using std::find;
    return optional_iterator{
      v, find(v.begin(), v.end(), std::forward<W>(what))};
}
//------------------------------------------------------------------------------
/// @brief Erases elements satisfying the specified predicate from a static_vector.
/// @ingroup container
/// @relates static_vector
export template <typename T, std::size_t N, typename A, typename Predicate>
auto erase_if(small_vector<T, N, A>& v, const Predicate& predicate) noexcept ->
  typename small_vector<T, N, A>::size_type {
    return v.erase_if(predicate);
}
//------------------------------------------------------------------------------
} // namespace eagine
