/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;
#include <cassert>

export module eagine.core.types:optional_reference;

import std;
import :concepts;
import :basic;
import :tribool;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Optional reference to an instance of type @p T.
/// @ingroup valid_if
/// @see valid_if
export template <typename T>
class optional_reference {

    template <typename V, typename... Args>
    [[nodiscard]] static constexpr auto _call_member(
      V* ptr,
      auto function,
      Args&&... args) noexcept(noexcept(function)) {
        using R = std::invoke_result_t<
          decltype(function),
          std::conditional_t<std::is_const_v<V>, std::add_const_t<T>, T>&,
          Args...>;
        if constexpr(std::is_reference_v<R> or std::is_pointer_v<R>) {
            using P = std::conditional_t<
              std::is_reference_v<R>,
              std::remove_reference_t<R>,
              std::remove_pointer_t<R>>;
            if(ptr) {
                return optional_reference<P>{
                  std::invoke(function, *ptr, std::forward<Args>(args)...)};
            } else {
                return optional_reference<P>{nothing};
            }
        } else {
            using O = std::remove_cvref_t<R>;
            if(ptr) {
                return std::optional<O>{
                  std::invoke(function, *ptr, std::forward<Args>(args)...)};
            } else {
                return std::optional<O>{};
            }
        }
    }

public:
    /// @brief Type of the referenced value.
    using value_type = T;

    /// @brief Construction from a pointer to reference of type @p T.
    constexpr optional_reference(T* ptr) noexcept
      : _ptr{ptr} {}

    /// @brief Construction from a reference to reference of type @p T.
    constexpr optional_reference(T& ref) noexcept
      : _ptr{&ref} {}

    template <typename U>
        requires(std::is_convertible_v<U*, T*> and not std::is_same_v<U, T>)
    constexpr optional_reference(optional_reference<U> that) noexcept
      : _ptr{that.get()} {}

    /// @brief Construction from a unique_ptr to reference of type @p T.
    template <std::derived_from<T> U>
    constexpr optional_reference(const std::unique_ptr<U>& ptr) noexcept
      : _ptr{ptr.get()} {}

    /// @brief Construction from a shared_ptr to reference of type @p T.
    template <std::derived_from<T> U>
    constexpr optional_reference(const std::shared_ptr<U>& ptr) noexcept
      : _ptr{ptr.get()} {}

    /// @brief Move constructor.
    constexpr optional_reference(optional_reference&&) noexcept = default;

    /// @brief Copy constructor.
    constexpr optional_reference(const optional_reference&) = default;

    /// @brief Move assignment operator.
    constexpr auto operator=(optional_reference&&) noexcept
      -> optional_reference& = default;

    /// @brief Copy assignment operator.
    constexpr auto operator=(const optional_reference&)
      -> optional_reference& = default;

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
    [[nodiscard]] constexpr auto has_value() const noexcept -> bool {
        return _ptr != nullptr;
    }

    /// @brief Resets the reference to invalid state.
    /// @post not has_value()
    constexpr auto reset() noexcept -> optional_reference& {
        _ptr = nullptr;
        return *this;
    }

    /// @brief Indicates in this reference specified to the specified object.
    [[nodiscard]] constexpr auto refers_to(const T& object) const noexcept
      -> tribool {
        return tribool{_ptr == &object, has_value()};
    }

    /// @brief Indicates in this reference the same object as @p that.
    template <std::derived_from<T> U>
    [[nodiscard]] constexpr auto refers_to(
      const optional_reference<U>& that) const noexcept -> tribool {
        if(that.has_value()) {
            return refers_to(*that);
        }
        return indeterminate;
    }

    /// @brief Indicates if this stores a valid reference.
    /// @see has_value
    [[nodiscard]] constexpr explicit operator bool() const noexcept {
        return has_value();
    }

    [[nodiscard]] constexpr auto operator*() const noexcept -> T& {
        assert(has_value());
        return *_ptr;
    }

    [[nodiscard]] constexpr auto operator->() const noexcept -> T* {
        assert(has_value());
        return _ptr;
    }

    [[nodiscard]] constexpr auto get() const noexcept -> T* {
        return _ptr;
    }

    /// @brief Returns the stored reference.
    /// @pre has_value()
    [[nodiscard]] constexpr auto value() const noexcept -> T& {
        assert(has_value());
        return *_ptr;
    }

    /// @brief Returns the stored value if valid or @p fallback otherwise.
    /// @see has_value
    template <
      std::convertible_to<T> U,
      typename R = std::conditional_t<std::is_function_v<T>, T*, T>>
    [[nodiscard]] constexpr auto value_or(U&& fallback) const noexcept -> R {
        if(has_value()) {
            if constexpr(std::is_function_v<T>) {
                return _ptr;
            } else {
                return *_ptr;
            }
        }
        return R(std::forward<U>(fallback));
    }

    [[nodiscard]] constexpr auto value_or(T& fallback) const noexcept -> T& {
        if(has_value()) {
            return *_ptr;
        }
        return fallback;
    }

    [[nodiscard]] explicit constexpr operator T&() noexcept {
        return value();
    }

    /// @brief Constructs value of type C from the stored value or an empty optional.
    /// @see and_then
    template <typename C, typename... Args>
    [[nodiscard]] constexpr auto construct(Args&&... args) noexcept(
      noexcept(T(std::declval<T&>()))) -> std::optional<C> {
        if(has_value()) {
            return {C{value(), std::forward<Args>(args)...}};
        }
        return {};
    }

    /// @brief Invoke function on the stored value or return empty optional-like.
    /// @see transform
    /// @see or_else
    template <
      typename F,
      optional_like R = std::remove_cvref_t<std::invoke_result_t<F, T&>>>
    constexpr auto and_then(F&& function) const noexcept(noexcept(
      std::invoke(std::forward<F>(function), std::declval<T&>()))) -> R {
        if(has_value()) {
            return std::invoke(std::forward<F>(function), *_ptr);
        } else {
            return R{};
        }
    }

    template <
      typename F,
      typename R = std::remove_cvref_t<std::invoke_result_t<F, T&>>>
        requires(std::is_void_v<R>)
    constexpr void and_then(F&& function) const noexcept(
      noexcept(std::invoke(std::forward<F>(function), std::declval<T&>()))) {
        if(has_value()) {
            std::invoke(std::forward<F>(function), *_ptr);
        }
    }

    template <typename F>
    constexpr auto and_then_true(F&& function) const noexcept(noexcept(
      std::invoke(std::forward<F>(function), std::declval<T&>()))) -> tribool {
        if(has_value()) {
            std::invoke(std::forward<F>(function), *_ptr);
            return true;
        } else {
            return indeterminate;
        }
    }

    /// @brief Return self if has value or the result of function.
    /// @see and_then
    /// @see transform
    template <typename F, typename R = std::invoke_result_t<F>>
        requires(
          std::same_as<R, optional_reference> or
          std::convertible_to<R, optional_reference>)
    constexpr auto or_else(F&& function) const
      noexcept(noexcept(std::invoke(std::forward<F>(function))))
        -> optional_reference {
        if(has_value()) {
            return *this;
        } else {
            return std::invoke(std::forward<F>(function));
        }
    }

    /// @brief Invoke function on the stored value or return empty optional-like.
    /// @see and_then
    /// @see or_else
    template <typename F, typename R = std::invoke_result_t<F, T&>>
    [[nodiscard]] constexpr auto transform(F&& function) const noexcept(
      noexcept(std::invoke(std::forward<F>(function), std::declval<T&>())) and
      std::is_nothrow_move_constructible_v<std::remove_cvref_t<R>>) {
        if constexpr(std::is_reference_v<R> or std::is_pointer_v<R>) {
            using P = std::conditional_t<
              std::is_reference_v<R>,
              std::remove_reference_t<R>,
              std::remove_pointer_t<R>>;
            if(_ptr) {
                return optional_reference<P>{
                  std::invoke(std::forward<F>(function), *_ptr)};
            } else {
                return optional_reference<P>{nothing};
            }
        } else {
            using O = std::remove_cvref_t<R>;
            if(_ptr) {
                return std::optional<O>{
                  std::invoke(std::forward<F>(function), *_ptr)};
            } else {
                return std::optional<O>{};
            }
        }
    }

    template <typename M, typename C>
        requires(std::is_same_v<std::remove_cv_t<C>, std::remove_cv_t<T>>)
    [[nodiscard]] constexpr auto member(M C::*ptr) const noexcept
        requires(not std::is_member_function_pointer_v<decltype(ptr)>)
    {
        using R =
          std::conditional_t<std::is_const_v<T>, std::add_const_t<M>, M>;
        if(has_value()) {
            return optional_reference<R>{value().*ptr};
        } else {
            return optional_reference<R>{nothing};
        }
    }

    template <typename R, std::same_as<T> C, typename... Params, typename... Args>
    [[nodiscard]] constexpr auto member(
      R (C::*function)(Params...),
      Args&&... args) const {
        return _call_member(_ptr, function, std::forward<Args>(args)...);
    }

    template <typename R, std::same_as<T> C, typename... Params, typename... Args>
    [[nodiscard]] constexpr auto member(
      R (C::*function)(Params...) noexcept,
      Args&&... args) const noexcept {
        return _call_member(_ptr, function, std::forward<Args>(args)...);
    }

    template <typename R, typename C, typename... Params, typename... Args>
        requires(std::is_same_v<std::remove_cv_t<C>, std::remove_cv_t<T>>)
    [[nodiscard]] constexpr auto member(
      R (C::*function)(Params...) const,
      Args&&... args) const {
        return _call_member(_ptr, function, std::forward<Args>(args)...);
    }

    template <typename R, typename C, typename... Params, typename... Args>
        requires(std::is_same_v<std::remove_cv_t<C>, std::remove_cv_t<T>>)
    [[nodiscard]] constexpr auto member(
      R (C::*function)(Params...) const noexcept,
      Args&&... args) const noexcept {
        return _call_member(_ptr, function, std::forward<Args>(args)...);
    }

    template <std::derived_from<T> Derived>
    [[nodiscard]] auto as(std::type_identity<Derived> = {}) const noexcept
      -> optional_reference<Derived> {
        if(auto derived{dynamic_cast<Derived*>(_ptr)}) {
            return {derived};
        }
        return {};
    }

    /// @brief Returns the stored reference.
    /// @see value
    [[nodiscard]] explicit constexpr operator const T&() const noexcept {
        return value();
    }

    /// @brief Tri-state equality comparison of the referred instance with a value.
    [[nodiscard]] auto constexpr operator==(const T& r) const noexcept
      -> tribool {
        if(has_value()) {
            return value() == r;
        }
        return indeterminate;
    }

    /// @brief Tri-state nonequality comparison of the referred instance with a value.
    [[nodiscard]] constexpr auto operator!=(const T& r) const noexcept
      -> tribool {
        if(has_value()) {
            return value() != r;
        }
        return indeterminate;
    }

    /// @brief Conversion to std::optional
    [[nodiscard]] constexpr operator std::optional<std::reference_wrapper<T>>()
      const noexcept {
        if(has_value()) {
            return {value()};
        }
        return {};
    }

private:
    T* _ptr{nullptr};
};
//------------------------------------------------------------------------------
export template <typename T>
optional_reference(T* ptr) -> optional_reference<T>;

export template <typename T>
optional_reference(T& ptr) -> optional_reference<T>;

export template <typename T>
optional_reference(const std::unique_ptr<T>& ptr) -> optional_reference<T>;

export template <typename T>
optional_reference(const std::shared_ptr<T>& ptr) -> optional_reference<T>;
//------------------------------------------------------------------------------
} // namespace eagine
