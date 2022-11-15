/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.container:small_vector;

import :static_vector;
import <cstdint>;
export import <vector>;
export import <variant>;

namespace eagine {
//------------------------------------------------------------------------------
export template <typename T, std::size_t N, typename Allocator = std::allocator<T>>
class small_vector {
    using T0 = static_vector<T, N>;
    using T1 = std::vector<T, Allocator>;

public:
    using value_type = std::remove_const_t<T>;
    using pointer = T*;
    using const_pointer = const value_type*;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using allocator_type = std::allocator<T>;

    constexpr small_vector() noexcept = default;

    constexpr auto max_size() const noexcept -> size_type {
        if(std::holds_alternative<T0>(_storage)) {
            return std::get<T0>(_storage).max_size();
        } else {
            return std::get<T1>(_storage).max_size();
        }
    }

    constexpr auto capacity() const noexcept -> size_type {
        if(std::holds_alternative<T0>(_storage)) {
            return std::get<T0>(_storage).size();
        } else {
            return std::get<T1>(_storage).size();
        }
    }

    constexpr void reserve(size_type n) const noexcept {
        if(std::holds_alternative<T0>(_storage)) {
            std::get<T0>(_storage).reserve(n);
        } else {
            std::get<T1>(_storage).reserve(n);
        }
    }

    constexpr auto size() const noexcept -> size_type {
        if(std::holds_alternative<T0>(_storage)) {
            return std::get<T0>(_storage).size();
        } else {
            return std::get<T1>(_storage).size();
        }
    }

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

    constexpr auto empty() const noexcept -> bool {
        if(std::holds_alternative<T0>(_storage)) {
            return std::get<T0>(_storage).empty();
        } else {
            return std::get<T1>(_storage).empty();
        }
    }

    constexpr void clear() noexcept {
        if(std::holds_alternative<T0>(_storage)) {
            std::get<T0>(_storage).clear();
        } else {
            std::get<T1>(_storage).clear();
        }
    }

    constexpr auto data() noexcept -> pointer {
        if(std::holds_alternative<T0>(_storage)) {
            return std::get<T0>(_storage).data();
        } else {
            return std::get<T1>(_storage).data();
        }
    }

    constexpr auto data() const noexcept -> const_pointer {
        if(std::holds_alternative<T0>(_storage)) {
            return std::get<T0>(_storage).data();
        } else {
            return std::get<T1>(_storage).data();
        }
    }

    constexpr auto begin() noexcept -> iterator {
        return data();
    }

    constexpr auto begin() const noexcept -> const_iterator {
        return data();
    }

    constexpr auto cbegin() const noexcept -> const_iterator {
        return data();
    }

    constexpr auto end() noexcept -> iterator {
        return data() + size();
    }

    constexpr auto end() const noexcept -> const_iterator {
        return data() + size();
    }

    constexpr auto cend() const noexcept -> const_iterator {
        return data() + size();
    }

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

    constexpr void pop_back() noexcept {
        if(std::holds_alternative<T0>(_storage)) {
            std::get<T0>(_storage).pop_back();
        } else {
            std::get<T1>(_storage).pop_back();
        }
    }

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

    template <typename... Args>
    constexpr auto emplace(const_iterator pos, Args&&... args) -> iterator {
        const auto offs{std::distance(cbegin(), pos)};
        if(std::holds_alternative<T0>(_storage)) {
            auto& v0{std::get<T0>(_storage)};
            if(v0.full()) {
                _storage = T1(v0.begin(), v0.end());
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

    constexpr auto front() noexcept -> value_type& {
        if(std::holds_alternative<T0>(_storage)) {
            return std::get<T0>(_storage).front();
        } else {
            return std::get<T1>(_storage).front();
        }
    }

    constexpr auto front() const noexcept -> const value_type& {
        if(std::holds_alternative<T0>(_storage)) {
            return std::get<T0>(_storage).front();
        } else {
            return std::get<T1>(_storage).front();
        }
    }

    constexpr auto back() noexcept -> value_type& {
        if(std::holds_alternative<T0>(_storage)) {
            return std::get<T0>(_storage).back();
        } else {
            return std::get<T1>(_storage).back();
        }
    }

    constexpr auto back() const noexcept -> const value_type& {
        if(std::holds_alternative<T0>(_storage)) {
            return std::get<T0>(_storage).back();
        } else {
            return std::get<T1>(_storage).back();
        }
    }

    constexpr auto operator[](size_type i) noexcept -> value_type& {
        if(std::holds_alternative<T0>(_storage)) {
            return std::get<T0>(_storage)[i];
        } else {
            return std::get<T1>(_storage)[i];
        }
    }

    constexpr auto operator[](size_type i) const noexcept -> const value_type& {
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
export template <typename T, std::size_t N, typename A, typename Predicate>
auto erase_if(small_vector<T, N, A>& v, const Predicate& predicate) noexcept ->
  typename small_vector<T, N, A>::size_type {
    return v.erase_if(predicate);
}
//------------------------------------------------------------------------------
} // namespace eagine
