/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include <type_traits>
module;
#include <cassert>

export module eagine.core.types:optional_reference;

import eagine.core.concepts;
import :nothing;
import :tribool;
import <cstddef>;
import <concepts>;
import <functional>;
import <optional>;
import <utility>;
import <type_traits>;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Optional reference to an instance of type @p T.
/// @ingroup valid_if
/// @see valid_if
export template <typename T>
class optional_reference {
public:
    /// @brief Construction from a pointer to value of type @p T.
    optional_reference(T* ptr) noexcept
      : _ptr{ptr} {}

    /// @brief Construction from a reference to value of type @p T.
    optional_reference(T& ref) noexcept
      : _ptr{&ref} {}

    /// @brief Move constructor.
    optional_reference(optional_reference&&) noexcept = default;

    /// @brief Copy constructor.
    optional_reference(const optional_reference&) = default;

    /// @brief Move assignment operator.
    auto operator=(optional_reference&&) noexcept
      -> optional_reference& = default;

    /// @brief Copy assignment operator.
    auto operator=(const optional_reference&) -> optional_reference& = default;

    ~optional_reference() noexcept = default;

    /// @brief Default construction
    /// @post not has_value()
    constexpr optional_reference() noexcept = default;

    /// @brief Construction from nothing.
    /// @post not has_value()
    constexpr optional_reference(nothing_t) noexcept {}

    /// @brief Construction from nullptr.
    /// @post not has_value()
    constexpr optional_reference(std::nullptr_t) noexcept {}

    /// @brief Indicates if this stores a valid reference.
    [[nodiscard]] auto has_value() const noexcept -> bool {
        return _ptr != nullptr;
    }

    /// @brief Indicates if this stores a valid reference.
    /// @see has_value
    [[nodiscard]] explicit operator bool() const noexcept {
        return has_value();
    }

    /// @brief Returns reference to the stored value.
    /// @pre has_value()
    [[nodiscard]] auto value() & noexcept -> T& {
        assert(has_value());
        return *_ptr;
    }

    /// @brief Returns const reference to the stored value.
    /// @pre has_value()
    [[nodiscard]] auto value() const& noexcept -> const T& {
        assert(has_value());
        return *_ptr;
    }

    /// @brief Returns rvalue reference to the stored value.
    /// @pre has_value()
    [[nodiscard]] auto value() && noexcept -> T&& {
        assert(has_value());
        return std::move(*_ptr);
    }

    /// @brief Returns the stored value if valid or @p fallback otherwise.
    /// @see has_value
    template <typename U>
    [[nodiscard]] auto value_or(U&& fallback) const noexcept -> T
        requires(std::is_convertible_v<U, T>)
    {
        if(has_value()) {
            return *_ptr;
        }
        return T(std::forward<U>(fallback));
    }

    [[nodiscard]] explicit operator T&() noexcept {
        return value();
    }

    /// @brief Invoke function on the stored value or return empty extractable.
    /// @see transform
    template <typename F>
        requires(optional_like<std::remove_cvref_t<std::invoke_result_t<F, T&>>>)
    [[nodiscard]] auto and_then(F&& function) {
        using R = std::remove_cvref_t<std::invoke_result_t<F, T&>>;
        if(has_value()) {
            return std::invoke(std::forward<F>(function), this->value());
        } else {
            return R{};
        }
    }

    /// @brief Invoke function on the stored value or return empty extractable.
    /// @see and_then
    template <typename F>
    [[nodiscard]] auto transform(F&& function) {
        using R = std::invoke_result_t<F, T&>;
        if constexpr(std::is_reference_v<R> or std::is_pointer_v<R>) {
            using P = std::conditional_t<
              std::is_reference_v<R>,
              std::remove_reference_t<R>,
              std::remove_pointer_t<R>>;
            if(has_value()) {
                return optional_reference<P>{
                  std::invoke(std::forward<F>(function), this->value())};
            } else {
                return optional_reference<P>{nothing};
            }
        } else {
            using V = std::remove_cvref_t<R>;
            if(has_value()) {
                return std::optional<V>{
                  std::invoke(std::forward<F>(function), this->value())};
            } else {
                return std::optional<V>{};
            }
        }
    }

    /// @brief Invoke function on the stored value or return empty extractable.
    /// @see transform
    template <typename F>
        requires(
          optional_like<std::remove_cvref_t<std::invoke_result_t<F, const T&>>>)
    [[nodiscard]] auto and_then(F&& function) const {
        using R = std::remove_cvref_t<std::invoke_result_t<F, const T&>>;
        if(has_value()) {
            return std::invoke(std::forward<F>(function), this->value());
        } else {
            return R{};
        }
    }

    /// @brief Invoke function on the stored value or return empty extractable.
    /// @see and_then
    template <typename F>
    [[nodiscard]] auto transform(F&& function) const {
        using R = std::invoke_result_t<F, std::add_const_t<T>&>;
        if constexpr(std::is_reference_v<R> or std::is_pointer_v<R>) {
            using P = std::conditional_t<
              std::is_reference_v<R>,
              std::remove_reference_t<R>,
              std::remove_pointer_t<R>>;
            if(has_value()) {
                return optional_reference<P>{
                  std::invoke(std::forward<F>(function), this->value())};
            } else {
                return optional_reference<P>{nothing};
            }
        } else {
            using V = std::remove_cvref_t<R>;
            if(has_value()) {
                return std::optional<V>{
                  std::invoke(std::forward<F>(function), this->value())};
            } else {
                return std::optional<V>{};
            }
        }
    }

    template <typename M, std::same_as<T> C>
    [[nodiscard]] auto member(M C::*ptr) noexcept {
        if(has_value()) {
            return optional_reference<M>{this->value().*ptr};
        } else {
            return optional_reference<M>{nothing};
        }
    }

    template <typename M, typename C>
        requires(std::is_same_v<std::remove_cv_t<C>, std::remove_cv_t<T>>)
    [[nodiscard]] auto member(M C::*ptr) const noexcept {
        if(has_value()) {
            return optional_reference<std::add_const_t<M>>{this->value().*ptr};
        } else {
            return optional_reference<std::add_const_t<M>>{nothing};
        }
    }

    /// @brief Returns the stored reference.
    /// @see value
    [[nodiscard]] explicit operator const T&() const noexcept {
        return value();
    }

    /// @brief Tri-state equality comparison of the referred instance with a value.
    [[nodiscard]] auto operator==(const T& r) const noexcept -> tribool {
        if(has_value()) {
            return value() == r;
        }
        return indeterminate;
    }

    /// @brief Tri-state nonequality comparison of the referred instance with a value.
    [[nodiscard]] auto operator!=(const T& r) const noexcept -> tribool {
        if(has_value()) {
            return value() != r;
        }
        return indeterminate;
    }

private:
    T* _ptr{nullptr};
};
//------------------------------------------------------------------------------
export template <typename T>
struct extract_traits<optional_reference<T>> {
    using value_type = T;
    using result_type = T&;
    using const_result_type = std::add_const_t<T>&;
};

/// @brief Overload of extract for optional_reference.
/// @ingroup valid_if
export template <typename T>
[[nodiscard]] auto extract(optional_reference<T> ref) noexcept -> T& {
    return ref.value();
}
//------------------------------------------------------------------------------
} // namespace eagine
