/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module;
#include <cassert>

export module eagine.core.types:holder;

import std;
import :concepts;
import :basic;
import :tribool;
import :optional_like;

namespace eagine {
//------------------------------------------------------------------------------
export template <typename T>
struct hold_t {};

/// @brief Tag template used to specify type of objects held by basic_holder.
/// @see basic_holder
export template <typename T>
constexpr const hold_t<T> hold = {};

export template <typename T>
class weak_holder;
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

    template <typename U>
    using rebind = std::unique_ptr<U>;
};

template <typename T>
struct basic_holder_traits<std::shared_ptr<T>> {
    template <std::derived_from<T> D, typename... Args>
    static constexpr auto make(hold_t<D>, Args&&... args) noexcept(
      noexcept(std::make_shared<D>(std::forward<Args>(args)...))) {
        return std::shared_ptr<T>{
          std::make_shared<D>(std::forward<Args>(args)...)};
    }

    template <typename U>
    using rebind = std::shared_ptr<U>;
};
//------------------------------------------------------------------------------
/// @brief Wrapper for smart pointers providing optional-like monadic API
/// @see hold
/// @see unique_holder
/// @see shared_holder
template <typename Base, typename T>
class basic_holder : public optional_like_crtp<basic_holder<Base, T>, T> {
    using _traits = basic_holder_traits<Base>;
    using _opt_ref = optional_reference<T>;

    friend class weak_holder<T>;

    Base _base;

public:
    using value_type = T;

    template <typename U>
    using rebind = basic_holder<typename _traits::template rebind<U>, U>;

    /// @brief Default constructor.
    /// @post not has_value()
    constexpr basic_holder() noexcept = default;
    constexpr basic_holder(basic_holder&&) noexcept = default;
    constexpr basic_holder(const basic_holder&) noexcept = default;
    constexpr auto operator=(basic_holder&&) noexcept
      -> basic_holder& = default;
    constexpr auto operator=(const basic_holder&) noexcept
      -> basic_holder& = default;

    constexpr basic_holder(Base base) noexcept
      : _base{std::move(base)} {}

    template <std::derived_from<T> U>
    constexpr basic_holder(typename _traits::template rebind<U> derived) noexcept
      : _base{std::move(derived)} {}

    /// @brief Conversion from another, compatible holder object.
    template <typename Other, std::derived_from<T> D>
        requires(
          std::is_convertible_v<Other, Base> and
          not std::is_same_v<Other, Base>)
    constexpr basic_holder(basic_holder<Other, D>&& temp) noexcept
      : _base{std::move(temp).release()} {}

    template <typename Other, typename D>
        requires(
          std::is_convertible_v<Other, Base> and
          not std::is_same_v<Other, Base>)
    constexpr basic_holder(const basic_holder<Other, D>& that) noexcept
      : _base{that.base()} {}

    /// @brief Construction with held object of type T from the given arguments.
    /// @post has_value()
    template <typename... Args>
        requires(not std::is_abstract_v<T>)
    constexpr basic_holder(default_selector_t, Args&&... args) noexcept(
      noexcept(_traits::make(hold<T>, std::forward<Args>(args)...)))
      : _base{_traits::make(hold<T>, std::forward<Args>(args)...)} {}

    /// @brief Construction with held object of type D from the given arguments.
    /// @post has_value()
    template <std::derived_from<T> D, typename... Args>
        requires(not std::is_abstract_v<D>)
    constexpr basic_holder(hold_t<D> h, Args&&... args) noexcept(
      noexcept(_traits::make(h, std::forward<Args>(args)...)))
      : _base{_traits::make(h, std::forward<Args>(args)...)} {}

    /// @brief Emplaces object of type T from the given arguments in this holder.
    /// @see emplace_derived
    /// @see ensure
    template <typename... Args>
    auto emplace(Args&&... args) noexcept(noexcept(
      _traits::make(hold<T>, std::forward<Args>(args)...))) -> basic_holder& {
        _base = _traits::make(hold<T>, std::forward<Args>(args)...);
        return *this;
    }

    /// @brief Emplaces object of type D from the given arguments in this holder.
    /// @see emplace
    /// @see ensure_derived
    template <std::derived_from<T> D, typename... Args>
        requires(not std::is_abstract_v<D>)
    auto emplace_derived(hold_t<D> h, Args&&... args) noexcept(noexcept(
      _traits::make(h, std::forward<Args>(args)...))) -> basic_holder& {
        _base = _traits::make(h, std::forward<Args>(args)...);
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

    /// @brief Ensures object of type T constructed from argument is in this holder.
    /// @see ensure
    template <typename... Args>
    [[nodiscard]] auto operator()(Args&&... args) noexcept -> basic_holder& {
        return ensure(std::forward<Args>(args)...);
    }

    /// @brief Conversion to optional_reference to the held type.
    [[nodiscard]] operator optional_reference<T>() const noexcept {
        return {this->get()};
    }

    /// @brief Indicates if this holder holds an object.
    [[nodiscard]] auto has_value() const noexcept -> bool {
        return bool(_base);
    }

    /// @brief Returns a reference to the held object.
    [[nodiscard]] auto operator*() const noexcept -> T& {
        return *_base;
    }

    /// @brief Returns a pointer to the held object.
    [[nodiscard]] auto get() const noexcept -> T* {
        return _base.get();
    }

    /// @brief Returns a reference to the held object.
    /// @pre has_value()
    [[nodiscard]] auto value() const noexcept -> value_type& {
        assert(has_value());
        return *_base;
    }

    /// @brief Resets the held object.
    [[nodiscard]] auto reset() noexcept {
        return _base.reset();
    }

    [[nodiscard]] auto base() const& noexcept -> const Base& {
        return _base;
    }

    [[nodiscard]] auto release() && noexcept -> Base&& {
        return {std::move(_base)};
    }

    template <std::derived_from<T> D>
    [[nodiscard]] auto as(std::type_identity<D> = {}) && noexcept
      -> basic_holder<std::shared_ptr<D>, D> {
        return {std::dynamic_pointer_cast<D>(
          std::shared_ptr<T>{std::move(*this).release()})};
    }
};
//------------------------------------------------------------------------------
/// @brief Specialization of basic_holder wrapping unique pointers.
/// @see basic_holder
/// @see unique_keeper
export template <typename T>
using unique_holder = basic_holder<std::unique_ptr<T>, T>;
//------------------------------------------------------------------------------
/// @brief Specialization of unique_holder that holds an object when constructed.
/// @see unique_holder
export template <typename T>
class unique_keeper : public unique_holder<T> {
public:
    /// @brief Constructs with held instance constructed with specified parameters.
    template <typename... Args>
    constexpr unique_keeper(Args&&... args) noexcept
      : unique_holder<T> {
        hold<T>, std::forward<Args>(args)...
    }
    {}
};
//------------------------------------------------------------------------------
/// @brief Specialization of basic_holder wrapping shared pointers.
/// @see basic_holder
export template <typename T>
using shared_holder = basic_holder<std::shared_ptr<T>, T>;
//------------------------------------------------------------------------------
export template <typename T>
class weak_holder : private std::weak_ptr<T> {
    using _base = std::weak_ptr<T>;

public:
    constexpr weak_holder() noexcept = default;

    weak_holder(const shared_holder<T>& that) noexcept
      : _base{that.base()} {}

    [[nodiscard]] auto lock() const noexcept -> shared_holder<T> {
        return {_base::lock()};
    }
};
//------------------------------------------------------------------------------
} // namespace eagine
