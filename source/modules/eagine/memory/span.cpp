/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.memory:span;

import eagine.core.types;
import :address;

import <concepts>;
import <cstring>;
import <initializer_list>;
import <iterator>;
import <type_traits>;

namespace eagine {
namespace memory {
//------------------------------------------------------------------------------
// clang-format off
export template <typename T, typename P = anything, typename S = span_size_t>
concept span_source = requires(T v) {
	{ v.data() } -> std::convertible_to<P>;
    { v.size() } -> std::convertible_to<S>;
};
// clang-format on
//------------------------------------------------------------------------------
// rebind_pointer
//------------------------------------------------------------------------------
export template <typename Ptr, typename U>
struct rebind_pointer;
//------------------------------------------------------------------------------
export template <typename Ptr, typename U>
using rebind_pointer_t = typename rebind_pointer<Ptr, U>::type;
//------------------------------------------------------------------------------
export template <typename T, typename U>
struct rebind_pointer<T*, U> : std::type_identity<U*> {};
//------------------------------------------------------------------------------
// basic_span
//------------------------------------------------------------------------------
/// @brief Non-owning view of a contiguous range of memory with ValueType elements.
/// @ingroup type_utils
/// @tparam ValueType the type of elements assumed in the covered memory range.
/// @tparam Pointer the pointer type used to point to the start of the span.
/// @tparam SizeType the integer type used to count the elements in the span.
/// @see basic_block
/// @see basic_split_span
///
/// This template is similar to std::span but allows to specify other pointer
/// types besides ValueType*, for example basic_offset_ptr, etc. and to specify
/// the size type.
export template <
  typename ValueType,
  typename Pointer = ValueType*,
  typename SizeType = span_size_t>
class basic_span {
public:
    /// @brief The element value type.
    using value_type = ValueType;

    /// @brief The element count type.
    using size_type = SizeType;

    /// @brief The memory address type.
    using address_type = basic_address<std::is_const_v<ValueType>>;

    /// @brief The pointer type.
    using pointer = Pointer;

    /// @brief The iterator type.
    using iterator = Pointer;

    /// @brief The const iterator type.
    using const_iterator = Pointer;

    /// @brief The reverse iterator type
    using reverse_iterator = std::reverse_iterator<iterator>;

    /// @brief Construction from pointer and length.
    constexpr basic_span(
      pointer addr, // NOLINT(modernize-pass-by-value)
      const size_type len) noexcept
      : _size{len}
      , _addr{addr} {}

    /// @brief Construction from memory address and length.
    constexpr basic_span(const address_type addr, const size_type len) noexcept
      : basic_span{static_cast<pointer>(addr), len} {}

    /// @brief Construction from a pair of pointers.
    constexpr basic_span(pointer b, pointer e) noexcept
      : basic_span{b, b <= e ? e - b : 0} {}

    /// @brief Construction from a pair of memory addresses.
    constexpr basic_span(const address_type ba, const address_type be) noexcept
      : basic_span{
          static_cast<pointer>(ba),
          limit_cast<size_type>(
            (ba <= be) ? (be - ba) / sizeof(value_type) : 0)} {}

    /// @brief Default constructor. Constructs an empty span.
    /// @post is_empty()
    /// @post size() == 0
    constexpr basic_span() noexcept = default;

    /// @brief Copy constructor.
    constexpr basic_span(const basic_span&) noexcept = default;

    /// @brief Move constructor.
    constexpr basic_span(basic_span&&) noexcept = default;

    /// @brief Copy assignment operator.
    auto operator=(const basic_span&) noexcept -> basic_span& = default;

    /// @brief Move assignment operator.
    auto operator=(basic_span&&) noexcept -> basic_span& = default;
    ~basic_span() noexcept = default;

    /// @brief Converting copy constructor from span of compatible elements.
    template <typename T, typename P, typename S>
        requires(
          std::is_convertible_v<T, ValueType> &&
            std::is_convertible_v<P, Pointer> &&
            std::is_convertible_v<S, SizeType> &&
            !std::is_same_v<T, ValueType> ||
          !std::is_same_v<P, Pointer> || !std::is_same_v<S, SizeType>)
    constexpr basic_span(basic_span<T, P, S> that) noexcept
      : basic_span{
          static_cast<pointer>(that.data()),
          limit_cast<size_type>(that.size())} {}

    /// @brief Converting copy assignment from span of compatible elements.
    template <typename T, typename P, typename S>
        requires(std::is_convertible_v<T, ValueType> &&
                   std::is_convertible_v<P, Pointer> &&
                   std::is_convertible_v<S, SizeType> &&
                   !std::is_same_v<T, ValueType> ||
                 !std::is_same_v<P, Pointer> || !std::is_same_v<S, SizeType>)
    auto operator=(basic_span<T, P, S> that) noexcept -> auto& {
        _addr = static_cast<pointer>(that.data());
        _size = limit_cast<size_type>(that.size());
        return *this;
    }

    /// @brief Resets this span.
    /// @post is_empty()
    auto reset() noexcept -> auto& {
        return *this = {};
    }

    /// @brief Resets this span with a new pointer and length.
    auto reset(pointer addr, const size_type length) noexcept -> auto& {
        return *this = basic_span(addr, length);
    }

    /// @brief Resets this span with a new memory address and length.
    auto reset(const address_type addr, const size_type length) noexcept
      -> auto& {
        return *this = basic_span(addr, length);
    }

    /// @brief Resets this span with a pair or pointers.
    auto reset(pointer b, pointer e) noexcept -> auto& {
        return *this = basic_span(b, e);
    }

    /// @brief Indicates that the span is not empty.
    /// @see is_empty
    /// @see size
    explicit constexpr operator bool() const noexcept {
        return !is_empty();
    }

    /// @brief Indicates that the span is empty.
    /// @see size
    constexpr auto is_empty() const noexcept -> bool {
        return size() == 0;
    }

    /// @brief Indicates that the span is empty.
    /// @see is_empty
    /// @see size
    constexpr auto empty() const noexcept -> bool {
        return size() == 0;
    }

    /// @brief Indicates that the span is terminated with value T(0) if applicable.
    constexpr auto is_zero_terminated() const noexcept -> bool {
        return _size < 0;
    }

    /// @brief Returns the number of elements in the span.
    /// @see is_empty
    constexpr auto size() const noexcept -> size_type {
        if constexpr(std::is_same_v<std::remove_const_t<ValueType>, char>) {
            if(_size < 0) [[likely]] {
                return -_size;
            }
            return _size;
        } else {
            if(_size >= 0) [[likely]] {
                return _size;
            }
            return -_size;
        }
    }

    /// @brief Returns a pointer to the start of the span.
    /// @see begin
    /// @see end
    /// @see size
    constexpr auto data() const noexcept -> pointer {
        return _addr;
    }

    /// @brief Returns an interator to the start of the span.
    constexpr auto begin() const noexcept -> iterator {
        return _addr;
    }

    /// @brief Returns a iterator past the end of the span.
    constexpr auto end() const noexcept -> iterator {
        return begin() + size();
    }

    /// @brief Returns a reverse interator to the end of the span.
    constexpr auto rbegin() const noexcept {
        return reverse_iterator{end()};
    }

    /// @brief Returns a reverse interator past the begin of the span.
    constexpr auto rend() const noexcept {
        return reverse_iterator{begin()};
    }

    /// @brief Returns the memory address of the start of the span.
    constexpr auto addr() const noexcept -> address_type {
        return as_address(begin());
    }

    /// @brief Returns the memory address of the start of the span.
    constexpr auto begin_addr() const noexcept -> address_type {
        return as_address(begin());
    }

    /// @brief Returns the memory address past the end of the span.
    constexpr auto end_addr() const noexcept -> address_type {
        return as_address(end());
    }

    /// @brief Checks if the start of the span is aligned as the alignment of X.
    template <typename X>
    auto is_aligned_as() const noexcept -> bool {
        return addr().template is_aligned_as<X>();
    }

    /// @brief Indicates if this span encloses the specified address.
    /// @see contains
    auto encloses(const pointer p) const noexcept -> bool {
        return (begin() <= p) && (p <= end());
    }

    /// @brief Indicates if this span encloses the specified address.
    /// @see contains
    auto encloses(const const_address a) const noexcept -> bool {
        return (addr() <= a) && (a <= end_addr());
    }

    /// @brief Indicates if this span encloses another span.
    /// @see encloses
    /// @see overlaps
    template <typename Ts, typename Ps, typename Ss>
    auto contains(basic_span<Ts, Ps, Ss> that) const noexcept -> bool {
        return (addr() <= that.addr()) && (that.end_addr() <= end_addr());
    }

    /// @brief Indicates if this span overlaps with another span.
    /// @see encloses
    /// @see contains
    template <typename Ts, typename Ps, typename Ss>
    auto overlaps(const basic_span<Ts, Ps, Ss>& that) const noexcept -> bool {
        return encloses(that.addr()) || encloses(that.end_addr()) ||
               that.encloses(addr()) || that.encloses(end_addr());
    }

    /// @brief Returns a const reference to value at the specified index.
    /// @pre index < size()
    constexpr auto ref(const size_type index) const noexcept
      -> std::add_const_t<value_type>& {
        assert(index < size());
        return _addr[index];
    }

    /// @brief Returns a reference to value at the specified index.
    /// @pre index < size()
    auto ref(const size_type index) noexcept -> value_type& {
        assert(index < size());
        return _addr[index];
    }

    /// @brief Returns a const reference to value at the front of the span.
    /// @see back
    /// @pre !is_empty()
    auto front() const noexcept -> const value_type& {
        assert(0 < size());
        return _addr[0];
    }

    /// @brief Returns a reference to value at the front of the span.
    /// @see back
    /// @pre !is_empty()
    auto front() noexcept -> value_type& {
        assert(0 < size());
        return _addr[0];
    }

    /// @brief Returns a const reference to value at the back of the span.
    /// @see front
    /// @pre !is_empty()
    auto back() const noexcept -> const value_type& {
        assert(0 < size());
        return _addr[size() - 1];
    }

    /// @brief Returns a const reference to value at the back of the span.
    /// @see front
    /// @pre !is_empty()
    auto back() noexcept -> value_type& {
        assert(0 < size());
        return _addr[size() - 1];
    }

    /// @brief Returns a const reference to value at the specified index.
    /// @pre 0 <= index < size()
    template <typename Int>
    constexpr auto element(const Int index) const noexcept
      -> std::add_const_t<value_type>&
        requires(std::is_integral_v<Int>)
    {
        return ref(span_size(index));
    }

    /// @brief Returns a reference to value at the specified index.
    /// @pre 0 <= index < size()
    template <typename Int>
    auto element(const Int index) noexcept -> value_type&
        requires(std::is_integral_v<Int>)
    {
        return ref(span_size(index));
    }

    /// @brief Array subscript operator.
    /// @see element
    template <typename Int>
    auto operator[](const Int index) noexcept -> value_type&
        requires(std::is_integral_v<Int>)
    {
        return element(index);
    }

    /// @brief Array subscript operator.
    /// @see element
    template <typename Int>
    constexpr auto operator[](const Int index) const noexcept
      -> std::add_const_t<value_type>&
        requires(std::is_integral_v<Int>)
    {
        return element(index);
    }

private:
    // if _size is negative it indicates that the span is terminated
    // by a zero value. if such case the span length is calculated by
    // getting the absolute value of _size.
    size_type _size{0};
    pointer _addr{nullptr};
};
//------------------------------------------------------------------------------
/// @brief Converts argument to span using a native pointer type.
/// @ingroup memory
/// @see relative
export template <typename T, typename P, typename S>
constexpr auto absolute(const basic_span<T, P, S> spn) noexcept
  -> basic_span<T, T*, S> {
    return {spn};
}
//------------------------------------------------------------------------------
/// @brief Default alias for basic memory spans with native pointer type.
/// @ingroup memory
export template <typename T>
using span = basic_span<T, T*, span_size_t>;
//------------------------------------------------------------------------------
/// @brief Alias for span<T> if T is mutable type. Ill defined otherwise.
/// @ingroup memory
export template <typename T>
using span_if_mutable = std::enable_if_t<!std::is_const_v<T>, span<T>>;
//------------------------------------------------------------------------------
/// @brief Alias for spans with const element type.
/// @ingroup memory
export template <typename T>
using const_span = span<std::add_const_t<T>>;
//------------------------------------------------------------------------------
/// @brief Creates a single-const-element view over the specified value.
/// @ingroup memory
export template <typename T>
constexpr auto view_one(const T& value) noexcept -> const_span<T> {
    return {std::addressof(value), span_size(1)};
}
//------------------------------------------------------------------------------
/// @brief Creates a single-element mutable span over the specified value.
/// @ingroup memory
export template <typename T>
constexpr auto cover_one(T& value) noexcept -> span_if_mutable<T> {
    return {std::addressof(value), span_size(1)};
}
//------------------------------------------------------------------------------
/// @brief Creates a single-const-element view from the specified pointer.
/// @ingroup memory
export template <typename T>
constexpr auto view_one(const T* pointer) noexcept -> const_span<T> {
    return {pointer, span_size(1)};
}
//------------------------------------------------------------------------------
/// @brief Creates a single-element mutable span from the specified pointer.
/// @ingroup memory
export template <typename T>
constexpr auto cover_one(T* pointer) noexcept -> span_if_mutable<T> {
    return {pointer, span_size(1)};
}
//------------------------------------------------------------------------------
/// @brief Creates a view starting at the specified pointer and specified length.
/// @ingroup memory
export template <typename T, typename S>
constexpr auto view(T* addr, const S size) noexcept -> const_span<T> {
    return {addr, span_size(size)};
}
//------------------------------------------------------------------------------
/// @brief Creates a span starting at the specified pointer and specified length.
/// @ingroup memory
export template <typename T, typename S>
constexpr auto cover(T* addr, const S size) noexcept -> span_if_mutable<T> {
    return {addr, span_size(size)};
}
//------------------------------------------------------------------------------
/// @brief Creates a view starting at the specified address and specified length.
/// @ingroup memory
export template <typename T, typename S>
constexpr auto view(const_address addr, const S size) noexcept
  -> const_span<T> {
    return {addr, span_size(size)};
}
//------------------------------------------------------------------------------
/// @brief Creates a span starting at the specified address and specified length.
/// @ingroup memory
export template <typename T, typename S>
constexpr auto cover(address addr, const S size) noexcept
  -> span_if_mutable<T> {
    return {addr, span_size(size)};
}
//------------------------------------------------------------------------------
/// @brief Creates a const view over the specified fixed-size array.
/// @ingroup memory
export template <typename T, std::size_t N>
constexpr auto view(const T (&array)[N]) noexcept -> const_span<T> {
    return view(static_cast<const T*>(array), N);
}
//------------------------------------------------------------------------------
/// @brief Creates a mutable span covering the specified fixed-size array.
/// @ingroup memory
export template <typename T, std::size_t N>
constexpr auto cover(T (&array)[N]) noexcept -> span_if_mutable<T> {
    return cover(static_cast<T*>(array), N);
}
//------------------------------------------------------------------------------
/// @brief Creates a const view over the specified initializer list.
/// @ingroup memory
export template <typename T>
constexpr auto view(std::initializer_list<T> il) noexcept -> const_span<T> {
    return view(il.begin(), il.size());
}
//------------------------------------------------------------------------------
/// @brief Creates a const view over a compatible contiguous container.
/// @ingroup memory
export template <span_source C>
constexpr auto view(const C& container) noexcept {
    return view(container.data(), container.size());
}
//------------------------------------------------------------------------------
/// @brief Creates a mutable span covering a compatible contiguous container.
/// @ingroup memory
export template <span_source C>
constexpr auto cover(C& container) noexcept {
    return cover(container.data(), container.size());
}
//------------------------------------------------------------------------------
// accommodate
//------------------------------------------------------------------------------
export constexpr auto can_accommodate_between(
  const_address bgn,
  const_address end,
  const span_size_t size) noexcept -> bool {
    return (end - bgn) >= size;
}
//------------------------------------------------------------------------------
/// @brief Indicates if the specified memory block can accommodate count elements of T.
/// @ingroup type_utils
export template <typename T, typename B, typename P, typename S>
constexpr auto can_accommodate(
  const basic_span<B, P, S> blk,
  const span_size_t count,
  const std::type_identity<T> tid = {}) noexcept {
    return is_aligned_as(blk.begin_addr(), tid) &&
           can_accommodate_between(
             blk.begin_addr(), blk.end_addr(), count * span_size_of(tid));
}
//------------------------------------------------------------------------------
/// @brief Indicates if the specified memory block can accommodate one element of T.
/// @ingroup type_utils
export template <typename T, typename B, typename P, typename S>
constexpr auto can_accommodate(
  const basic_span<B, P, S> blk,
  const std::type_identity<T> tid = {}) noexcept {
    return can_accommodate(blk, 1, tid);
}
//------------------------------------------------------------------------------
/// @brief Returns a span, rebinding the element type (typically from basic_block).
/// @ingroup memory
/// @see as_bytes
/// @see as_chars
export template <typename T, typename B, typename P, typename S>
constexpr auto accommodate(
  const basic_span<B, P, S> blk,
  const std::type_identity<T> tid = {}) noexcept
  -> basic_span<T, rebind_pointer_t<P, T>, S> {
    assert(can_accommodate(blk, tid));
    return basic_span<T, rebind_pointer_t<P, T>, S>{
      blk.begin_addr(), blk.end_addr()};
}
//------------------------------------------------------------------------------
// extract
//------------------------------------------------------------------------------
export template <typename T, typename P, typename S>
constexpr auto has_value(basic_span<T, P, S> spn) noexcept -> bool {
    return spn.size() >= 1;
}

/// @brief Overload of extract for spans. Returns the first element,
/// @pre spn.size() >= 1
/// @ingroup memory
export template <typename T, typename P, typename S>
constexpr auto extract(basic_span<T, P, S> spn) noexcept -> T& {
    assert(spn.size() >= 1);
    return spn.front();
}
//------------------------------------------------------------------------------
// basic_chunk_span
//------------------------------------------------------------------------------
/// @brief Non-owning view of a contiguous range of memory with ValueType elements.
/// @ingroup type_utils
/// @tparam ValueType the type of elements assumed in the covered memory range.
/// @tparam Pointer the pointer type used to point to the start of the span.
/// @tparam SizeType the integer type used to count the elements in the span.
/// @see basic_span
/// @see basic_split_span
///
/// This template is similar to std::span but allows to specify other pointer
/// types besides ValueType*, for example basic_offset_ptr, etc. and to specify
/// the size type.
export template <
  typename ValueType,
  typename Pointer,
  typename SizeType,
  SizeType chunkSize>
class basic_chunk_span : public basic_span<ValueType, Pointer, SizeType> {
    using base = basic_span<ValueType, Pointer, SizeType>;

public:
    constexpr basic_chunk_span() noexcept = default;
    constexpr basic_chunk_span(base that) noexcept
      : base{that} {}
};
//------------------------------------------------------------------------------
/// @brief Default alias for basic memory chunk spans with native pointer type.
/// @ingroup memory
template <typename T, span_size_t chunkSize>
using chunk_span = basic_chunk_span<T, T*, span_size_t, chunkSize>;
//------------------------------------------------------------------------------
} // namespace memory
//------------------------------------------------------------------------------
export template <typename>
struct extract_traits;

export template <typename T, typename P, typename S>
struct extract_traits<memory::basic_span<T, P, S>> {
    using value_type = T;
    using result_type = T&;
    using const_result_type = std::add_const_t<T>&;
};

export template <
  typename Tl,
  typename Tr,
  typename Pl,
  typename Pr,
  typename Sl,
  typename Sr>
struct equal_cmp<memory::basic_span<Tl, Pl, Sl>, memory::basic_span<Tr, Pr, Sr>> {
    static auto check(
      const memory::basic_span<Tl, Pl, Sl> l,
      const memory::basic_span<Tr, Pr, Sr> r) noexcept -> bool {
        if(are_equal(l.size(), r.size())) {
            if constexpr(
              std::is_same_v<std::remove_const_t<Tl>, std::remove_const_t<Tr>> &&
              std::is_integral_v<std::remove_const_t<Tl>>) {
                return 0 ==
                       std::memcmp(
                         l.data(), r.data(), sizeof(Tl) * std_size(l.size()));
            } else {
                for(const auto i : integer_range(span_size(l.size()))) {
                    if(!are_equal(l[i], r[i])) {
                        return false;
                    }
                }
                return true;
            }
        }
        return false;
    }
};
//------------------------------------------------------------------------------
export using memory::cover;
export using memory::cover_one;
export using memory::span;
export using memory::view;
export using memory::view_one;
//------------------------------------------------------------------------------
} // namespace eagine
