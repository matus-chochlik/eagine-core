/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.memory:address;

import eagine.core.types;
import :align;
import <cstddef>;
import <cstdint>;
import <type_traits>;

namespace eagine::memory {
//------------------------------------------------------------------------------
/// @brief Class for handling memory addresses as integer values.
/// @ingroup memory
export template <bool IsConst>
class basic_address {
public:
    /// @brief The associated untyped pointer type.
    using pointer = std::conditional_t<IsConst, const void*, void*>;

    /// @brief The associated byte pointer type.
    using byte_pointer = std::conditional_t<IsConst, const byte*, byte*>;

    ~basic_address() noexcept = default;

    /// @brief Default constructor.
    constexpr basic_address() noexcept = default;

    /// @brief Move constructor.
    constexpr basic_address(basic_address&&) noexcept = default;

    /// @brief Move assignment operator.
    constexpr auto operator=(basic_address&&) noexcept
      -> basic_address& = default;

    /// @brief Copy constructor.
    constexpr basic_address(const basic_address&) noexcept = default;

    /// @brief Copy assignment operator.
    constexpr auto operator=(const basic_address&) noexcept
      -> basic_address& = default;

    /// @brief Construction from nullptr.
    constexpr explicit basic_address(std::nullptr_t) noexcept {}

    /// @brief Construction from signed integer type.
    constexpr explicit basic_address(const std::intptr_t addr) noexcept
      : _addr{reinterpret_cast<pointer>(addr)} {}

    /// @brief Construction from untyped pointer type.
    constexpr explicit basic_address(pointer addr) noexcept
      : _addr{addr} {}

    template <typename Int>
    constexpr basic_address(basic_address that, Int offs) noexcept
        requires(
          std::is_integral_v<Int> && std::is_convertible_v<Int, std::ptrdiff_t>)
    // NOLINTNEXTLINE(performance-no-int-to-ptr)
    : _addr{reinterpret_cast<pointer>(that.value() + offs)} {}

    template <bool IsConst2>
    constexpr basic_address(basic_address<IsConst2> a) noexcept
        requires(IsConst && !IsConst2)
    : _addr{pointer(a)} {}

    /// @brief Indicates if the stored address is null.
    constexpr auto is_null() const noexcept {
        return _addr == nullptr;
    }

    /// @brief Indicates if the stored address is not null.
    /// @see is_null
    constexpr explicit operator bool() const noexcept {
        return !is_null();
    }

    /// @brief Returns the byte pointer for this address.
    /// @see get
    /// @see value
    constexpr auto ptr() const noexcept {
        return static_cast<byte_pointer>(_addr);
    }

    /// @brief Returns the untyped pointer for this address.
    /// @see ptr
    /// @see value
    constexpr auto get() const noexcept {
        return _addr;
    }

    /// @brief Explicit cast to the untyped pointer for this address.
    /// @see ptr
    constexpr explicit operator pointer() const noexcept {
        return _addr;
    }

    /// @brief Explicit cast to a typed pointer for this address.
    /// @see ptr
    /// @see is_aligned_as
    /// @pre is_aligned_as<T>()
    template <typename T>
    constexpr explicit operator T*() const noexcept
        requires(!std::is_void_v<T> && (std::is_const_v<T> || !IsConst))
    {
        assert(is_aligned_as<T>());
        return static_cast<T*>(_addr);
    }

    /// @brief Returns this address as an signed integer.
    /// @see ptr
    /// @see get
    constexpr auto value() const noexcept {
        return reinterpret_cast<std::intptr_t>(_addr);
    }

    /// @brief Returns the misalignment of this address to the specified alignment.
    /// @see is_aligned_to
    /// @see is_aligned_as
    constexpr auto misalignment(const span_size_t alignment) const noexcept {
        return memory::misalignment(value(), alignment);
    }

    /// @brief Indicates if this address is aligned to the specified alignment.
    /// @see misalignment
    /// @see is_aligned_as
    constexpr auto is_aligned_to(const span_size_t alignment) const noexcept {
        return memory::is_aligned_to(value(), alignment);
    }

    /// @brief Indicates if this address is aligned to the alignment or type T.
    /// @see misalignment
    /// @see is_aligned_to
    template <typename T>
    constexpr auto is_aligned_as(
      const std::type_identity<T> tid = {}) const noexcept {
        return memory::is_aligned_as<T>(value(), tid);
    }

    /// @brief Equality comparison.
    constexpr friend auto operator==(
      const basic_address a,
      const basic_address b) noexcept {
        return a.ptr() == b.ptr();
    }

    /// @brief Non-equality comparison.
    constexpr friend auto operator!=(
      const basic_address a,
      const basic_address b) noexcept {
        return a.ptr() != b.ptr();
    }

    /// @brief Less-than comparison.
    constexpr friend auto operator<(
      const basic_address a,
      const basic_address b) noexcept {
        return a.ptr() < b.ptr();
    }

    /// @brief Less-equal comparison.
    constexpr friend auto operator<=(
      const basic_address a,
      const basic_address b) noexcept {
        return a.ptr() <= b.ptr();
    }

    /// @brief Greater-than comparison.
    constexpr friend auto operator>(
      const basic_address a,
      const basic_address b) noexcept {
        return a.ptr() > b.ptr();
    }

    /// @brief Greater-equal comparison.
    constexpr friend auto operator>=(
      const basic_address a,
      const basic_address b) noexcept {
        return a.ptr() >= b.ptr();
    }

    /// @brief Byte difference between two addresses.
    constexpr friend auto operator-(
      const basic_address a,
      const basic_address b) noexcept -> std::ptrdiff_t {
        return a.ptr() - b.ptr();
    }

    /// @brief Byte offset addition.
    constexpr friend auto operator+(
      const basic_address a,
      const std::ptrdiff_t o) noexcept -> basic_address {
        return {a, o};
    }

    /// @brief Byte offset subtraction.
    constexpr friend auto operator-(
      const basic_address a,
      const std::ptrdiff_t o) noexcept -> basic_address {
        return {a, -o};
    }

private:
    pointer _addr{nullptr};
};
//------------------------------------------------------------------------------
/// @brief Type alias for const memory address values.
/// @ingroup memory
export using const_address = basic_address<true>;

/// @brief Type alias for non-const memory address values.
/// @ingroup memory
export using address = basic_address<false>;
//------------------------------------------------------------------------------
/// @brief Casts a pointer to basic_address.
/// @ingroup memory
export template <typename T>
constexpr auto as_address(T* addr) noexcept {
    return basic_address<std::is_const_v<T>>(addr);
}
//------------------------------------------------------------------------------
/// @brief Returns the misalignment of a memory address to the specified alignment.
/// @ingroup memory
export constexpr auto misalignment(
  const_address addr,
  const span_size_t alignment) noexcept {
    return addr.misalignment(alignment);
}
//------------------------------------------------------------------------------
/// @brief Returns the misalignment of a pointer to the specified alignment.
/// @ingroup memory
export constexpr auto misalignment(
  const void* ptr,
  const span_size_t alignment) noexcept {
    return misalignment(as_address(ptr), alignment);
}
//------------------------------------------------------------------------------
/// @brief Indicates if a memory address aligned to the specified alignment.
/// @ingroup memory
export constexpr auto is_aligned_to(
  const_address addr,
  const span_size_t alignment) noexcept {
    return addr.is_aligned_to(alignment);
}
//------------------------------------------------------------------------------
/// @brief Indicates if a pointer is aligned to the specified alignment.
/// @ingroup memory
export constexpr auto is_aligned_to(
  const void* ptr,
  const span_size_t alignment) noexcept {
    return is_aligned_to(as_address(ptr), alignment);
}
//------------------------------------------------------------------------------
/// @brief Indicates if a memory address aligned as the specified type T.
/// @ingroup memory
export template <typename T>
constexpr auto is_aligned_as(
  const_address addr,
  const std::type_identity<T> tid = {}) noexcept {
    return addr.is_aligned_as(tid);
}
//------------------------------------------------------------------------------
constexpr auto align_up_offs(
  span_size_t align,
  const span_size_t misalign) noexcept {
    return (misalign ? align - misalign : 0);
}
//------------------------------------------------------------------------------
export template <bool IsConst>
constexpr auto align_up_by(
  const basic_address<IsConst> addr,
  const span_size_t offs,
  const span_size_t max) noexcept {
    assert(offs <= max);
    return basic_address<IsConst>(addr, offs);
}
//------------------------------------------------------------------------------
/// @brief Aligns a memory address up to the specified alignment.
/// @param max The maximum byte offset that can be applied to the address.
/// @ingroup memory
export template <bool IsConst>
constexpr auto align_up(
  const basic_address<IsConst> addr,
  const span_size_t align,
  const span_size_t max) noexcept -> basic_address<IsConst> {
    return align_up_by(
      addr, align_up_offs(align, misalignment(addr, align)), max);
}
//------------------------------------------------------------------------------
constexpr auto align_down_offs(
  const span_size_t,
  const span_size_t misalign) noexcept {
    return misalign;
}
//------------------------------------------------------------------------------
export template <bool IsConst>
constexpr auto align_down_by(
  const basic_address<IsConst> addr,
  const span_size_t offs,
  const span_size_t max) noexcept {
    assert(offs <= max);
    return basic_address<IsConst>(addr, -offs);
}
//------------------------------------------------------------------------------
/// @brief Aligns a memory address down to the specified alignment.
/// @param max The maximum byte offset that can be applied to the address.
/// @ingroup memory
export template <bool IsConst>
constexpr auto align_down(
  const basic_address<IsConst> addr,
  const span_size_t align,
  const span_size_t max) noexcept -> basic_address<IsConst> {

    return align_down_by(
      addr, align_down_offs(align, misalignment(addr, align)), max);
}
//------------------------------------------------------------------------------
/// @brief Aligns a memory address down to the specified alignment.
/// @ingroup memory
export template <bool IsConst>
constexpr auto align_down(
  const basic_address<IsConst> addr,
  const span_size_t align) noexcept {
    return align_down(addr, align, addr.value());
}
//------------------------------------------------------------------------------
/// @brief Aligns a byte pointer down to the specified alignment.
/// @ingroup memory
export inline auto align_down(const byte* ptr, const span_size_t align)
  -> const byte* {
    return align_down(const_address(ptr), align).ptr();
}
//------------------------------------------------------------------------------
/// @brief Aligns a memory address up to the required alignment of type T.
/// @param max The maximum byte offset that can be applied to the address.
/// @ingroup memory
export template <typename T>
constexpr auto align_up_to(
  const basic_address<std::is_const_v<T>> addr,
  const std::type_identity<T> = {},
  span_size_t align = span_align_of<T>(),
  const span_size_t max = span_size_of<T>()) noexcept {

    if(align < span_align_of<T>()) {
        align = span_align_of<T>();
    }

    return static_cast<T*>(align_up(addr, align, max));
}
//------------------------------------------------------------------------------
/// @brief Aligns a memory address down to the required alignment of type T.
/// @param max The maximum byte offset that can be applied to the address.
/// @ingroup memory
export template <typename T>
constexpr auto align_down_to(
  const basic_address<std::is_const_v<T>> addr,
  const std::type_identity<T> = {},
  span_size_t align = span_align_of<T>(),
  const span_size_t max = span_size_of<T>()) noexcept {
    if(align < span_align_of<T>()) {
        align = span_align_of<T>();
    }

    return static_cast<T*>(align_down(addr, align, max));
}
//------------------------------------------------------------------------------
} // namespace eagine::memory
