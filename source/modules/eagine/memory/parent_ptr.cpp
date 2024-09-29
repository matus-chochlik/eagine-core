/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.memory:parent_ptr;

import std;
import eagine.core.types;
import :address;

namespace eagine::memory {
//------------------------------------------------------------------------------
export template <typename Parent, std::signed_integral OffsetType>
class basic_parent_ptr {
public:
    /// @brief The pointee value type.
    using value_type = Parent;

    /// @brief The non-const address types.
    using address = basic_address<std::is_const_v<Parent>>;

    /// @brief The const address types.
    using const_address = basic_address<true>;

    /// @brief The offset type.
    using offset_type = OffsetType;

    /// @brief The non-const pointer type.
    using pointer = Parent*;

    /// @brief The const pointer type.
    using const_pointer = std::add_const_t<Parent>*;

    /// @brief The non-const reference type.
    using reference = Parent&;

    /// @brief The const reference type.
    using const_reference = std::add_const_t<Parent>&;

    /// @brief Construction from the reference to the parent object.
    basic_parent_ptr(Parent& parent) noexcept
      : _offs{_get_offs(parent)} {}

    /// @brief Returns the byte offset value.
    constexpr auto offset() const noexcept -> offset_type {
        return _offs;
    }

    /// @brief Returns the pointed-to address.
    auto addr() noexcept -> address {
        return address(_this_addr(), _offs);
    }

    /// @brief Returns the pointed-to address.
    constexpr auto addr() const noexcept -> const_address {
        return address(_this_addr(), _offs);
    }

    /// @brief Returns the stored pointer.
    auto data() noexcept -> pointer {
        return static_cast<pointer>(addr());
    }

    /// @brief Returns the stored pointer.
    constexpr auto data() const noexcept -> const_pointer {
        return static_cast<const_pointer>(addr());
    }

    /// @brief Returns the stored pointer. Alias for data().
    /// @see data
    auto get() noexcept -> pointer {
        return data();
    }

    /// @brief Returns the stored pointer. Alias for data().
    /// @see data
    constexpr auto get() const noexcept -> const_pointer {
        return data();
    }

    /// @brief Implicit conversion to pointer type.
    /// @see get
    operator pointer() noexcept {
        return get();
    }

    /// @brief Implicit conversion to const pointer type.
    /// @see get
    constexpr operator const_pointer() const noexcept {
        return get();
    }

    /// @brief Dereferences this pointer.
    auto operator*() noexcept -> reference {
        return *get();
    }

    /// @brief Dereferences this pointer.
    constexpr auto operator*() const noexcept -> const_reference {
        return *get();
    }

    /// @brief Dereferences this pointer.
    auto operator->() noexcept -> pointer {
        return get();
    }

    /// @brief Dereferences this pointer.
    constexpr auto operator->() const noexcept -> const_pointer {
        return get();
    }

private:
    using _rawptr = typename address::pointer;

    auto _this_addr() const noexcept {
        return address(_rawptr(this));
    }

    auto _get_offs(address addr) noexcept -> offset_type {
        return addr ? addr - _this_addr() : 0;
    }

    auto _get_offs(Parent& parent) noexcept -> offset_type {
        return _get_offs(address(&parent));
    }

    offset_type _offs;
};
//------------------------------------------------------------------------------
} // namespace eagine::memory

namespace std {
export template <typename P, typename O>
struct pointer_traits<eagine::memory::basic_parent_ptr<P, O>> {
    using value_type = P;
    using difference_type = O;

    static constexpr auto to_address(
      eagine::memory::basic_parent_ptr<P, O> p) noexcept
      -> std::add_const_t<P>* {
        return p.get();
    }
};

} // namespace std
