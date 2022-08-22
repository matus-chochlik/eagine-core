/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.memory:alloc_arena;

import eagine.core.types;
import :address;
import :span;
import :block;
import :byte_allocator;
import <algorithm>;
import <utility>;
import <vector>;
import <stdexcept>;

namespace eagine::memory {

export template <typename Alloc>
class basic_allocation_arena : public block_owner {

public:
    basic_allocation_arena() = default;

    template <typename... P>
    explicit basic_allocation_arena(P&&... p)
      : _alloc{std::forward<P>(p)...} {}

    explicit basic_allocation_arena(Alloc&& alloc)
      : _alloc{std::move(alloc)} {}

    basic_allocation_arena(basic_allocation_arena&&) noexcept = default;
    basic_allocation_arena(const basic_allocation_arena&) = delete;
    auto operator=(basic_allocation_arena&&) = delete;
    auto operator=(const basic_allocation_arena&) = delete;

    ~basic_allocation_arena() noexcept {
        clear();
    }

    auto empty() const noexcept -> bool {
        assert(_blks.empty() == _alns.empty());
        return _blks.empty();
    }

    void clear();

    auto allocate(const span_size_t size, const span_size_t align) -> block {
        _blks.push_back(_alloc.allocate(size, align));
        _alns.push_back(align);
        return _blks.back();
    }

    template <typename T>
    auto make_aligned_array(const span_size_t count, const span_size_t align)
      -> span<T>;

    template <typename T>
    auto make_array(const span_size_t count) {
        return make_aligned_array<T>(count, 1);
    }

    template <typename T, typename P, typename S>
    auto copy_aligned_array(
      const basic_span<T, P, S> src,
      const span_size_t align) -> span<std::remove_const_t<T>> {
        auto dst =
          make_aligned_array<std::remove_const_t<T>>(src.size(), align);
        std::copy(src.begin(), src.end(), dst.begin());
        return dst;
    }

    template <typename T, typename P, typename S>
    auto copy_array(const basic_span<T, P, S> src)
      -> span<std::remove_const_t<T>> {
        return copy_aligned_array<T>(src, span_size(alignof(T)));
    }

    template <typename T, typename... Args>
    auto make_aligned(const span_size_t align, Args&&... args) -> T&;

    template <typename T, typename... Args>
    auto make(Args&&... args) -> T& {
        return make_aligned<T>(
          span_size(alignof(T)), std::forward<Args>(args)...);
    }

    template <typename T>
    void destroy(T& v) {
        v.~T();
    }

protected:
    auto allocator() const -> const Alloc& {
        return _alloc;
    }

private:
    Alloc _alloc;
    std::vector<owned_block> _blks;
    std::vector<span_size_t> _alns;

    auto _do_allocate(const span_size_t size, const span_size_t align) -> block;

    template <typename T>
    auto _allocate(const span_size_t count, const span_size_t align) -> block;

    template <typename T>
    auto _make_n(const span_size_t count, const span_size_t align) -> T*;

    template <typename T, typename... Args>
    auto _make_1(const span_size_t align, Args&&... args) -> T*;
};

using system_allocation_arena = basic_allocation_arena<c_byte_reallocator>;
//------------------------------------------------------------------------------
template <typename Alloc>
auto basic_allocation_arena<Alloc>::_do_allocate(
  const span_size_t size,
  const span_size_t align) -> block {
    owned_block b = _alloc.allocate(size, align);

    if(b.empty()) {
        return {};
    }

    assert(is_aligned_to(b.addr(), align));
    assert(b.size() >= size);

    _blks.push_back(std::move(b));
    _alns.push_back(align);

    return _blks.back();
}
//------------------------------------------------------------------------------
template <typename Alloc>
void basic_allocation_arena<Alloc>::clear() {
    assert(_blks.size() == _alns.size());

    for(std_size_t i = 0; i < _blks.size(); ++i) {
        _alloc.deallocate(std::move(_blks[i]), _alns[i]);
    }
    _blks.clear();
    _alns.clear();
}
//------------------------------------------------------------------------------
template <typename Alloc>
template <typename T>
auto basic_allocation_arena<Alloc>::_allocate(
  const span_size_t count,
  const span_size_t align) -> block {
    return _do_allocate(
      span_size_of<T>(count), std::max(align, span_align_of<T>()));
}
//------------------------------------------------------------------------------
template <typename Alloc>
template <typename T>
auto basic_allocation_arena<Alloc>::_make_n(
  const span_size_t count,
  const span_size_t align) -> T* {
    return new(_allocate<T>(count, align).data()) T[integer(count)]{};
}
//------------------------------------------------------------------------------
template <typename Alloc>
template <typename T, typename... Args>
auto basic_allocation_arena<Alloc>::_make_1(const span_size_t align, Args&&... a)
  -> T* {
    return new(_allocate<T>(1, align).data()) T(std::forward<Args>(a)...);
}
//------------------------------------------------------------------------------
template <typename Alloc>
template <typename T>
auto basic_allocation_arena<Alloc>::make_aligned_array(
  const span_size_t count,
  const span_size_t align) -> span<T> {
    if(count < 1) {
        return {};
    }

    T* p = _make_n<T>(count, align);
    if(!p) {
        throw std::bad_alloc();
    }
    return {p, count};
}
//------------------------------------------------------------------------------
template <typename Alloc>
template <typename T, typename... Args>
auto basic_allocation_arena<Alloc>::make_aligned(
  const span_size_t align,
  Args&&... args) -> T& {
    T* p = _make_1<T>(align, std::forward<Args>(args)...);
    if(!p) {
        throw std::bad_alloc();
    }

    return *p;
}
//------------------------------------------------------------------------------
} // namespace eagine::memory

