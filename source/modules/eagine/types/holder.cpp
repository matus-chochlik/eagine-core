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

export module eagine.core.types:holder;

import std;
import :concepts;
import :basic;
import :tribool;
import :optional_reference;

namespace eagine {
//------------------------------------------------------------------------------
export template <typename T>
struct hold_t {};

/// @brief Tag template used to specify type of objects held by basic_holder.
/// @see basic_holder
export template <typename T>
constexpr const hold_t<T> hold = {};
//------------------------------------------------------------------------------
template <typename Base>
struct basic_holder_traits;

template <typename T>
struct basic_holder_traits<std::unique_ptr<T>> {
    template <std::derived_from<T> D, typename... Args>
    static constexpr auto make(hold_t<D>, Args&&... args) noexcept(
      noexcept(std::make_unique<D>(std::forward<Args>(args)...))) {
        return std::unique_ptr<T>{
          std::make_unique<D>(std::forward<Args>(args)...)};
    }
};

template <typename T>
struct basic_holder_traits<std::shared_ptr<T>> {
    template <std::derived_from<T> D, typename... Args>
    static constexpr auto make(hold_t<D>, Args&&... args) noexcept(
      noexcept(std::make_shared<D>(std::forward<Args>(args)...))) {
        return std::shared_ptr<T>{
          std::make_shared<D>(std::forward<Args>(args)...)};
    }
};
//------------------------------------------------------------------------------
/// @brief Wrapper for smart pointers providing optional-like monadic API
/// @see hold
/// @see unique_holder
/// @see shared_holder
template <typename Base, typename T>
class basic_holder : private Base {
    using _traits = basic_holder_traits<Base>;

public:
    using value_type = T;

    /// @brief Default constructor.
    /// @post not has_value()
    constexpr basic_holder() noexcept = default;

    constexpr basic_holder(Base base) noexcept
      : Base{std::move(base)} {}

    /// @brief Conversion from another, compatible holder object.
    template <typename Other, std::derived_from<T> D>
        requires(
          std::is_convertible_v<Other, Base> and
          not std::is_same_v<Other, Base>)
    constexpr basic_holder(basic_holder<Other, D>&& temp) noexcept
      : Base{std::move(temp).release()} {}

    /// @brief Construction with held object of type D from the given arguments.
    /// @post has_value()
    template <std::derived_from<T> D, typename... Args>
        requires(not std::is_abstract_v<D>)
    constexpr basic_holder(hold_t<D> h, Args&&... args) noexcept(
      noexcept(_traits::make(h, std::forward<Args>(args)...)))
      : Base{_traits::make(h, std::forward<Args>(args)...)} {}

    using Base::operator bool;

    /// @brief Conversion to optional_reference to the held_type.
    [[nodiscard]] operator optional_reference<T>() const noexcept {
        return {this->get()};
    }

    /// @brief Indicates if this holder holds an object.
    [[nodiscard]] auto has_value() const noexcept -> bool {
        return Base::operator bool();
    }

    /// @brief Returns a reference to the held object.
    /// @pre has_value()
    [[nodiscard]] auto operator*() const noexcept -> value_type& {
        assert(has_value());
        return Base::operator*();
    }

    /// @brief Returns a pointer to the held object.
    /// @pre has_value()
    [[nodiscard]] auto operator->() const noexcept -> value_type* {
        assert(has_value());
        return Base::operator->();
    }

    /// @brief Returns a reference to the held object.
    /// @pre has_value()
    [[nodiscard]] auto value() const noexcept -> value_type& {
        assert(has_value());
        return Base::operator*();
    }

    /// @brief Returns copy of the stored object if any or @p fallback otherwise.
    /// @see has_value
    template <std::convertible_to<T> U>
    [[nodiscard]] constexpr auto value_or(U&& fallback) const noexcept -> T {
        if(has_value()) {
            return Base::operator*();
        }
        return T(std::forward<U>(fallback));
    }

    /// @brief Returns reference to the stored object if any or @p fallback otherwise.
    /// @see has_value
    [[nodiscard]] constexpr auto value_or(value_type& fallback) const noexcept
      -> value_type& {
        if(has_value()) {
            return Base::operator*();
        }
        return fallback;
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
            return std::invoke(std::forward<F>(function), Base::operator*());
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
            std::invoke(std::forward<F>(function), Base::operator*());
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
            if(has_value()) {
                return optional_reference<P>{
                  std::invoke(std::forward<F>(function), Base::operator*())};
            } else {
                return optional_reference<P>{nothing};
            }
        } else if constexpr(std::is_same_v<R, bool>) {
            if(has_value()) {
                return tribool{
                  std::invoke(std::forward<F>(function), Base::operator*()),
                  true};
            } else {
                return tribool{indeterminate};
            }
        } else {
            if(has_value()) {
                return std::optional<R>{
                  std::invoke(std::forward<F>(function), Base::operator*())};
            } else {
                return std::optional<R>{};
            }
        }
    }

    using Base::reset;

    [[nodiscard]] auto release() && noexcept -> Base&& {
        return static_cast<Base&&>(*this);
    }
};
//------------------------------------------------------------------------------
/// @brief Specialization of basic_holder wrapping unique pointers.
/// @see basic_holder
export template <typename T>
using unique_holder = basic_holder<std::unique_ptr<T>, T>;
//------------------------------------------------------------------------------
/// @brief Specialization of basic_holder wrapping shared pointers.
/// @see basic_holder
export template <typename T>
using shared_holder = basic_holder<std::shared_ptr<T>, T>;
//------------------------------------------------------------------------------
} // namespace eagine
