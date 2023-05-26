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

import std;
import eagine.core.concepts;
import eagine.core.types;
import :address;

namespace eagine {
namespace memory {
//------------------------------------------------------------------------------
export template <typename T, typename P = anything, typename S = span_size_t>
concept span_source = requires(T& v) {
    { v.data() } -> std::convertible_to<P>;
    { v.size() } -> std::convertible_to<S>;
};
export template <typename T, typename P = anything, typename S = span_size_t>
concept const_span_source = requires(const T& v) {
    { v.data() } -> std::convertible_to<P>;
    { v.size() } -> std::convertible_to<S>;
};
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
export template <typename ValueType, typename Pointer, typename SizeType>
class basic_span {
public:
    /// @brief The value type.
    using value_type = std::remove_cv_t<ValueType>;
    //
    /// @brief The element type.
    using element_type = ValueType;

    /// @brief The element count type.
    using size_type = SizeType;

    /// @brief The memory address type.
    using address_type = basic_address<std::is_const_v<element_type>>;

    /// @brief The pointer type.
    using pointer = Pointer;

    /// @brief The iterator type.
    using iterator = typename std::span<element_type>::iterator;

    using const_iterator = iterator;

    /// @brief The reverse iterator type
    using reverse_iterator = typename std::span<element_type>::reverse_iterator;

    /// @brief Construction from pointer and length.
    constexpr basic_span(
      pointer addr, // NOLINT(modernize-pass-by-value)
      const size_type len) noexcept
      : _size{len}
      , _addr{addr} {}

    /// @brief Construction from memory address and length.
    constexpr basic_span(const address_type addr, const size_type len) noexcept
      : basic_span{static_cast<pointer>(addr), len} {}

    /// @brief Construction from a pair of pointers or iterators.
    template <typename I>
        requires(
          std::is_same_v<I, value_type*> or std::is_same_v<I, element_type*> or
          std::is_same_v<I, pointer> or std::is_same_v<I, iterator>)
    constexpr basic_span(I b, I e) noexcept
      : basic_span{b == e ? nullptr : &*b, std::distance(b, e)} {}

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
          std::is_convertible_v<T, ValueType> and
            std::is_convertible_v<P, Pointer> and
            std::is_convertible_v<S, SizeType> and
            not std::is_same_v<T, ValueType> or
          not std::is_same_v<P, Pointer> or not std::is_same_v<S, SizeType>)
    constexpr basic_span(basic_span<T, P, S> that) noexcept
      : basic_span{
          static_cast<pointer>(that.data()),
          limit_cast<size_type>(that.size())} {}

    /// @brief Converting copy assignment from span of compatible elements.
    template <typename T, typename P, typename S>
        requires(
          std::is_convertible_v<T, ValueType> and
            std::is_convertible_v<P, Pointer> and
            std::is_convertible_v<S, SizeType> and
            not std::is_same_v<T, ValueType> or
          not std::is_same_v<P, Pointer> or not std::is_same_v<S, SizeType>)
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
        return size() >= 1;
    }

    /// @brief Indicates that the span is empty.
    /// @see has_single_value
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

    /// @brief Indicates if the span has exactly one value.
    /// @see is_empty
    /// @see size
    /// @see has_value
    constexpr auto has_single_value() const noexcept -> bool {
        return size() == 1;
    }

    /// @brief Indicates if the span has exactly one value.
    /// @see is_empty
    /// @see has_single_value
    constexpr auto has_value() const noexcept -> bool {
        return has_single_value();
    }

    /// @brief Indicates that the span is terminated with value T(0) if applicable.
    constexpr auto is_zero_terminated() const noexcept -> bool {
        return _size < 0;
    }

    constexpr auto zero_terminated_size() const noexcept -> size_type {
        return _size;
    }

    /// @brief Returns the number of elements in the span.
    /// @see is_empty
    /// @see std_size
    constexpr auto size() const noexcept -> size_type {
        return std::abs(_size);
    }

    /// @brief Returns the number of elements in the span as std::size.
    /// @see is_empty
    /// @see size
    constexpr auto std_size() const noexcept -> std::size_t {
        return static_cast<std::size_t>(std::abs(_size));
    }

    /// @brief Returns a pointer to the start of the span.
    /// @see begin
    /// @see end
    /// @see size
    constexpr auto data() const noexcept -> pointer {
        return _addr;
    }

    constexpr auto data_end() const noexcept -> pointer {
        return data() + size();
    }

    constexpr auto std_span() const noexcept -> std::span<element_type> {
        using std::to_address;
        return {to_address(_addr), std_size()};
    }

    /// @brief Returns an interator to the start of the span.
    constexpr auto begin() const noexcept {
        return std_span().begin();
    }

    /// @brief Returns a iterator past the end of the span.
    constexpr auto end() const noexcept {
        return std_span().end();
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
        return as_address(_addr);
    }

    /// @brief Returns the memory address of the start of the span.
    constexpr auto begin_addr() const noexcept -> address_type {
        return as_address(_addr);
    }

    /// @brief Returns the memory address past the end of the span.
    constexpr auto end_addr() const noexcept -> address_type {
        return as_address(_addr + size());
    }

    /// @brief Checks if the start of the span is aligned as the alignment of X.
    template <typename X>
    auto is_aligned_as() const noexcept -> bool {
        return addr().template is_aligned_as<X>();
    }

    /// @brief Indicates if this span encloses the specified address.
    /// @see contains
    auto encloses(const pointer p) const noexcept -> bool {
        return (begin() <= p) and (p <= end());
    }

    /// @brief Indicates if this span encloses the specified address.
    /// @see contains
    auto encloses(const const_address a) const noexcept -> bool {
        return (addr() <= a) and (a <= end_addr());
    }

    /// @brief Indicates if this span encloses another span.
    /// @see encloses
    /// @see overlaps
    template <typename Ts, typename Ps, typename Ss>
    auto contains(basic_span<Ts, Ps, Ss> that) const noexcept -> bool {
        return (addr() <= that.addr()) and (that.end_addr() <= end_addr());
    }

    /// @brief Indicates if this span overlaps with another span.
    /// @see encloses
    /// @see contains
    template <typename Ts, typename Ps, typename Ss>
    auto overlaps(const basic_span<Ts, Ps, Ss>& that) const noexcept -> bool {
        return encloses(that.addr()) or encloses(that.end_addr()) or
               that.encloses(addr()) or that.encloses(end_addr());
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
    auto ref(const size_type index) noexcept -> element_type& {
        assert(index < size());
        return _addr[index];
    }

    /// @brief Returns a const reference to value at the front of the span.
    /// @see back
    /// @pre not is_empty()
    auto front() const noexcept -> std::add_const_t<element_type>& {
        assert(not empty());
        return _addr[0];
    }

    /// @brief Returns a reference to value at the front of the span.
    /// @see back
    /// @pre not is_empty()
    auto front() noexcept -> element_type& {
        assert(not empty());
        return _addr[0];
    }

    /// @brief Returns a const reference to value at the back of the span.
    /// @see front
    /// @pre not is_empty()
    auto back() const noexcept -> std::add_const_t<element_type>& {
        assert(not empty());
        return _addr[size() - 1];
    }

    /// @brief Returns a const reference to value at the back of the span.
    /// @see front
    /// @pre not is_empty()
    auto back() noexcept -> element_type& {
        assert(not empty());
        return _addr[size() - 1];
    }

    /// @brief Returns the single value if size == 1
    /// @see front
    /// @pre has_single_value()
    auto value() const noexcept -> element_type {
        assert(has_single_value());
        return front();
    }

    /// @brief Returns the single value if size == 1 or fallback
    /// @see size
    /// @see front
    /// @see has_single_value
    template <std::convertible_to<element_type> U>
    auto value_or(U&& fallback) const noexcept -> element_type {
        if(has_single_value()) {
            return front();
        }
        return element_type(std::forward<U>(fallback));
    }

    /// @brief Returns the single value if size == 1 or default constructed element_type
    /// @see has_single_value
    /// @see size
    /// @see front()
    auto or_default() const noexcept -> element_type {
        if(has_single_value()) {
            return front();
        }
        return element_type{};
    }

    /// @brief Calls the specified function is size == 1
    /// @see has_single_value
    /// @see front()
    template <
      typename F,
      optional_like R =
        std::remove_cvref_t<std::invoke_result_t<F, const element_type&>>>
    auto and_then(F&& function) -> R {
        if(has_single_value()) {
            return std::invoke(std::forward<F>(function), front());
        } else {
            return R{};
        }
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
    auto element(const Int index) noexcept -> element_type&
        requires(std::is_integral_v<Int>)
    {
        return ref(span_size(index));
    }

    /// @brief Returns the single value if size == 1
    /// @see value
    /// @pre has_value()
    auto operator*() const noexcept -> element_type {
        return value();
    }

    /// @brief Array subscript operator.
    /// @see element
    template <typename Int>
    auto operator[](const Int index) noexcept -> element_type&
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
using span_if_mutable = std::enable_if_t<not std::is_const_v<T>, span<T>>;
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
export template <const_span_source C>
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
    return is_aligned_as(blk.begin_addr(), tid) and
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
    requires(
      std::is_same_v<std::remove_const_t<B>, char> or
      std::is_same_v<std::remove_const_t<B>, byte>)
constexpr auto accommodate(
  const basic_span<B, P, S> blk,
  const std::type_identity<T> tid = {}) noexcept
  -> basic_span<T, rebind_pointer_t<P, T>, S> {
    assert(blk.is_empty() or can_accommodate(blk, tid));
    return basic_span<T, rebind_pointer_t<P, T>, S>{
      blk.begin_addr(), blk.end_addr()};
}
//------------------------------------------------------------------------------
// extract
//------------------------------------------------------------------------------
/// @brief Overload of extract for spans. Returns the first element,
/// @pre spn.size() >= 1
/// @ingroup memory
export template <typename T, typename P, typename S>
constexpr auto extract(basic_span<T, P, S> spn) noexcept -> T& {
    assert(spn.has_single_value());
    return spn.front();
}

export template <typename T, typename P, typename S, typename Dst>
constexpr auto assign_if_fits(basic_span<T, P, S> src, Dst& dst) noexcept
  -> bool {
    if(src.has_single_value()) {
        return eagine::assign_if_fits(extract(src), dst);
    }
    return false;
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
export template <typename T, span_size_t chunkSize>
using chunk_span = basic_chunk_span<T, T*, span_size_t, chunkSize>;
//------------------------------------------------------------------------------
// block
//------------------------------------------------------------------------------
/// @brief Alias for byte spans.
/// @ingroup type_utils
export template <bool IsConst>
using basic_block = span<std::conditional_t<IsConst, const byte, byte>>;
//------------------------------------------------------------------------------
/// @brief Alias for non-const byte memory span.
/// @ingroup memory
/// @see buffer
export using block = basic_block<false>;

/// @brief Alias for const byte memory span.
/// @ingroup memory
/// @see buffer
export using const_block = basic_block<true>;
//------------------------------------------------------------------------------
/// @brief Converts a span into a basic_block.
/// @ingroup memory
/// @see accommodate
/// @see as_chars
export template <typename T, typename P, typename S>
constexpr auto as_bytes(const basic_span<T, P, S> spn) noexcept
  -> basic_block<std::is_const_v<T>> {
    return {spn.begin_addr(), spn.end_addr()};
}
//------------------------------------------------------------------------------
/// @brief Converts a block into a span of characters.
/// @ingroup memory
/// @see accommodate
/// @see as_bytes
export constexpr auto as_chars(const block blk) noexcept {
    return accommodate<char>(blk);
}
//------------------------------------------------------------------------------
/// @brief Converts a block into a span of characters.
/// @ingroup memory
/// @see accommodate
/// @see as_bytes
export constexpr auto as_chars(const const_block blk) noexcept {
    return accommodate<const char>(blk);
}
//------------------------------------------------------------------------------
export class block_owner;
//------------------------------------------------------------------------------
/// @brief Specialization of block indicating byte span ownership.
/// @ingroup memory
/// @see block
/// @see block_owner
export class owned_block : public block {
public:
    /// @brief Default constructor.
    /// @post is_empty()
    constexpr owned_block() noexcept = default;

    /// @brief Move constructor.
    owned_block(owned_block&& temp) noexcept
      : block{static_cast<const block&>(temp)} {
        temp.reset();
    }

    /// @brief Move assignment operator.
    auto operator=(owned_block&& temp) noexcept -> owned_block& {
        if(this != std::addressof(temp)) {
            static_cast<block&>(*this) = static_cast<const block&>(temp);
            temp.reset();
        }
        return *this;
    }

    /// @brief Not copy constructible.
    owned_block(const owned_block&) = delete;

    /// @brief Not copy assignable.
    auto operator=(const owned_block&) = delete;

    /// @brief Destructor.
    /// @pre is_empty()
    ~owned_block() noexcept {
        assert(empty());
    }

private:
    friend class block_owner;

    constexpr owned_block(block b) noexcept
      : block{b} {}
};
//------------------------------------------------------------------------------
/// @brief Base class for classes that act as memory block owners.
/// @ingroup memory
/// @see owned_block
export class block_owner {
protected:
    /// @brief Should be called to take the ownership of a memory block.
    [[nodiscard]] static auto acquire_block(block b) noexcept -> owned_block {
        return {b};
    }

    /// @brief Should be called to release the ownership of a memory block.
    static void release_block(owned_block&& b) noexcept {
        b.reset();
    }
};
//------------------------------------------------------------------------------
// structured block
//------------------------------------------------------------------------------
export template <typename T>
class structured_block {
private:
    basic_block<std::is_const_v<T>> _blk;

    template <typename X = T>
    auto _ptr() noexcept
        requires(not std::is_const_v<X>)
    {
        assert(is_valid_block(_blk));
        return static_cast<X*>(_blk.addr());
    }

    auto _cptr() const noexcept {
        assert(is_valid_block(_blk));
        return static_cast<const T*>(_blk.addr());
    }

public:
    static auto is_valid_block(const const_block blk) noexcept -> bool {
        return not blk.empty() and (blk.is_aligned_as<T>()) and
               (can_accommodate(blk, std::type_identity<T>()));
    }

    structured_block(basic_block<std::is_const_v<T>> blk) noexcept
      : _blk{blk} {
        assert(is_valid_block(_blk));
    }

    template <typename X = T>
    auto get() noexcept
      -> X& requires(not std::is_const_v<X> and std::is_same_v<X, T>) {
          return *_ptr();
      }

    template <typename X = T>
    auto operator->() noexcept
      -> X* requires(not std::is_const_v<X> and std::is_same_v<X, T>) {
          return _ptr();
      }

    auto get() const noexcept -> const T& {
        return *_cptr();
    }

    auto operator->() const noexcept -> const T* {
        return _cptr();
    }
};
//------------------------------------------------------------------------------
} // namespace memory
//------------------------------------------------------------------------------
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
              std::is_same_v<std::remove_const_t<Tl>, std::remove_const_t<Tr>> and
              std::is_integral_v<std::remove_const_t<Tl>>) {
                return 0 == std::memcmp(
                              l.data(), r.data(), sizeof(Tl) * l.std_size());
            } else {
                for(const auto i : integer_range(span_size(l.size()))) {
                    if(not are_equal(l[i], r[i])) {
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
