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
    using _opt_ref = optional_reference<T>;

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

    /// @brief Emplaces object of type T from the given arguments in this holder.
    /// @see emplace_derived
    /// @see ensure
    template <typename... Args>
    auto emplace(Args&&... args) noexcept(noexcept(
      _traits::make(hold<T>, std::forward<Args>(args)...))) -> basic_holder& {
        static_cast<Base&>(*this) =
          _traits::make(hold<T>, std::forward<Args>(args)...);
        return *this;
    }

    /// @brief Emplaces object of type D from the given arguments in this holder.
    /// @see emplace
    /// @see ensure_derived
    template <std::derived_from<T> D, typename... Args>
        requires(not std::is_abstract_v<D>)
    auto emplace_derived(hold_t<D> h, Args&&... args) noexcept(noexcept(
      _traits::make(h, std::forward<Args>(args)...))) -> basic_holder& {
        static_cast<Base&>(*this) =
          _traits::make(h, std::forward<Args>(args)...);
        return *this;
    }

    /// @brief Ensures object of type T construted from argument is in this holder.
    /// @see emplace
    /// @see ensure_derived
    template <typename... Args>
    auto ensure(Args&&... args) noexcept -> basic_holder& {
        if(not has_value()) [[unlikely]] {
            try {
                return emplace(std::forward<Args>(args)...);
            } catch(...) {
            }
        }
        return *this;
    }

    /// @brief Ensures object of type D construted from argument is in this holder.
    /// @see emplace_derived
    /// @see ensure
    template <std::derived_from<T> D, typename... Args>
    auto ensure_derived(hold_t<D> h, Args&&... args) noexcept -> basic_holder& {
        if(not has_value()) [[unlikely]] {
            try {
                return emplace_derived(h, std::forward<Args>(args)...);
            } catch(...) {
            }
        }
        return *this;
    }

    /// @brief Ensures object of type T construted from argument is in this holder.
    /// @see ensure
    template <typename... Args>
    [[nodiscard]] auto operator()(Args&&... args) noexcept -> basic_holder& {
        return ensure(std::forward<Args>(args)...);
    }

    using Base::operator bool;

    /// @brief Gets optional_reference to the held type.
    [[nodiscard]] auto ref() const noexcept -> optional_reference<T> {
        return {this->get()};
    }

    /// @brief Conversion to optional_reference to the held type.
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
        return ref().value_or(std::forward<U>(fallback));
    }

    /// @brief Returns reference to the stored object if any or @p fallback otherwise.
    /// @see has_value
    [[nodiscard]] constexpr auto value_or(value_type& fallback) const noexcept
      -> value_type& {
        return ref().value_or(fallback);
    }

    /// @brief Invoke function on the stored value or return empty optional-like.
    /// @see transform
    /// @see or_else
    template <typename F>
    constexpr auto and_then(F&& function) const noexcept(
      noexcept(std::declval<_opt_ref>().and_then(std::forward<F>(function)))) {
        return ref().and_then(std::forward<F>(function));
    }

    /// @brief Return optional_reference if has value or the result of function.
    /// @see and_then
    /// @see transform
    template <typename F>
    constexpr auto or_else(F&& function) const noexcept(
      noexcept(std::declval<_opt_ref>().or_else(std::forward<F>(function)))) {
        return ref().or_else(std::forward<F>(function));
    }

    /// @brief Invoke function on the stored value or return empty optional-like.
    /// @see and_then
    /// @see or_else
    template <typename F, typename R = std::invoke_result_t<F, T&>>
    [[nodiscard]] constexpr auto transform(F&& function) const
      noexcept(noexcept(std::declval<_opt_ref>().transform(function))) {
        return ref().transform(std::forward<F>(function));
    }

    [[nodiscard]] constexpr auto member(auto ptr) const noexcept
        requires(std::is_member_pointer_v<decltype(ptr)>)
    {
        return ref().member(ptr);
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