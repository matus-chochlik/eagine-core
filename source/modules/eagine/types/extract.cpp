/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.types:extract;

import eagine.core.concepts;
import <memory>;
import <optional>;

namespace eagine {
//------------------------------------------------------------------------------
export template <extractable T>
constexpr auto extract_or(
  T& opt_val,
  extract_result_type_t<T> fallback = {}) noexcept -> extract_result_type_t<T> {
    return has_value(opt_val) ? extract(opt_val) : fallback;
}

export template <extractable T>
constexpr auto extract_or(
  const T& opt_val,
  const_extract_result_type_t<T> fallback = {}) noexcept
  -> const_extract_result_type_t<T> {
    return has_value(opt_val) ? extract(opt_val) : fallback;
}
//------------------------------------------------------------------------------
// pointers
export template <typename T>
struct extract_traits<T*> {
    using value_type = T;
    using result_type = T&;
    using const_result_type = std::add_const_t<T>&;
};

/// @brief Checks @p ptr and dereferences it.
/// @pre has_value(ptr)
export template <typename T>
constexpr auto extract(T* ptr) noexcept -> T& {
    assert(ptr);
    return *ptr;
}
//------------------------------------------------------------------------------
export template <typename T>
struct extract_traits<std::shared_ptr<T>> {
    using value_type = T;
    using result_type = T&;
    using const_result_type = std::add_const_t<T>&;
};

/// @brief Checks @p ptr and dereferences it.
/// @pre has_value(ptr)
export template <typename T>
constexpr auto extract(std::shared_ptr<T>& ptr) noexcept -> auto& {
    assert(ptr);
    return *ptr;
}

/// @brief Checks @p ptr and dereferences it.
/// @pre has_value(ptr)
export template <typename T>
constexpr auto extract(const std::shared_ptr<T>& ptr) noexcept -> auto& {
    assert(ptr);
    return *ptr;
}
//------------------------------------------------------------------------------
export template <typename T, typename D>
struct extract_traits<std::unique_ptr<T, D>> {
    using value_type = T;
    using result_type = T&;
    using const_result_type = std::add_const_t<T>&;
};

/// @brief Checks @p ptr and dereferences it.
/// @pre has_value(ptr)
export template <typename T, typename D>
constexpr auto extract(std::unique_ptr<T, D>& ptr) noexcept -> T& {
    assert(ptr);
    return *ptr;
}

/// @brief Checks @p ptr and dereferences it.
/// @pre has_value(ptr)
export template <typename T, typename D>
constexpr auto extract(const std::unique_ptr<T, D>& ptr) noexcept -> const T& {
    assert(ptr);
    return *ptr;
}
//------------------------------------------------------------------------------
export template <typename T>
struct extract_traits<std::optional<T>> {
    using value_type = T;
    using result_type = T&;
    using const_result_type = std::add_const_t<T>&;
};

/// @brief Checks @p ptr and dereferences it.
/// @pre has_value(opt)
export template <typename T>
constexpr auto extract(std::optional<T>& opt) noexcept -> auto& {
    assert(opt);
    return *opt;
}

/// @brief Checks @p ptr and dereferences it.
/// @pre has_value(opt)
export template <typename T>
constexpr auto extract(const std::optional<T>& opt) noexcept -> const auto& {
    assert(opt);
    return *opt;
}
//------------------------------------------------------------------------------
} // namespace eagine
