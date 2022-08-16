/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

module eagine.core.memory;
import eagine.core.types;
import <memory>;

namespace eagine::memory {
//------------------------------------------------------------------------------
// stack_byte_allocator_only
//------------------------------------------------------------------------------
stack_byte_allocator_only::stack_byte_allocator_only(const block& blk)
  : _alloc{blk} {}
//------------------------------------------------------------------------------
auto stack_byte_allocator_only::max_size(size_type) noexcept -> size_type {
    return _alloc.max_size();
}
//------------------------------------------------------------------------------
auto stack_byte_allocator_only::has_allocated(
  const owned_block& b,
  span_size_t) noexcept -> tribool {
    return _alloc.has_allocated(b);
}
//------------------------------------------------------------------------------
auto stack_byte_allocator_only::equal(byte_allocator* a) const noexcept
  -> bool {
    auto* sba = dynamic_cast<stack_byte_allocator_only*>(a);

    return (sba != nullptr) && (this->_alloc == sba->_alloc);
}
//------------------------------------------------------------------------------
auto stack_byte_allocator_only::allocate(size_type n, size_type a) noexcept
  -> owned_block {
    size_type m = (a - _alloc.allocated_size() % a) % a;
    owned_block b = _alloc.allocate(m + n);

    if(b) {
        assert(is_aligned_to(b.begin() + m, a));
    }

    assert(m <= b.size());

    owned_block r{this->acquire_block({b.begin() + m, b.end()})};

    this->release_block(std::move(b));

    return r;
}
//------------------------------------------------------------------------------
void stack_byte_allocator_only::deallocate(owned_block&& b, size_type) noexcept {
    assert(_alloc.has_allocated(b));
    this->release_block(std::move(b));
}
//------------------------------------------------------------------------------
// stack_byte_allocator
//------------------------------------------------------------------------------
stack_byte_allocator::stack_byte_allocator(const block& blk)
  : _alloc{blk} {}
//------------------------------------------------------------------------------
auto stack_byte_allocator::equal(byte_allocator* a) const noexcept -> bool {
    auto* sba = dynamic_cast<stack_byte_allocator*>(a);

    return (sba != nullptr) && (this->_alloc == sba->_alloc);
}
//------------------------------------------------------------------------------
auto stack_byte_allocator::allocate(size_type n, size_type a) noexcept
  -> owned_block {
    size_type m = a - _alloc.allocated_size() % a;

    assert((m < 255) && "must fit into a byte");

    owned_block b = _alloc.allocate(m + n);

    if(b) {
        assert(is_aligned_to(b.begin() + m, a));
        b[m - 1] = byte(m);
    }

    assert(m <= b.size());

    owned_block r = this->acquire_block({b.begin() + m, b.end()});

    this->release_block(std::move(b));

    return r;
}
//------------------------------------------------------------------------------
void stack_byte_allocator::deallocate(owned_block&& b, size_type) noexcept {
    assert(_alloc.has_allocated(b));

    byte* p = b.data();

    assert(reinterpret_cast<std::intptr_t>(p) >= 1);

    const auto m = size_type(p[-1]);

    assert(reinterpret_cast<std::intptr_t>(p) >= m);

    _alloc.deallocate(this->acquire_block({p - m, safe_add(b.size(), m)}));

    this->release_block(std::move(b));
}
//------------------------------------------------------------------------------
// stack_aligned_byte_allocator
//------------------------------------------------------------------------------
stack_aligned_byte_allocator::stack_aligned_byte_allocator(
  const block& blk,
  span_size_t align)
  : _align{align}
  , _alloc{blk, _align} {}
//------------------------------------------------------------------------------
auto stack_aligned_byte_allocator::equal(byte_allocator* a) const noexcept
  -> bool {
    auto* sba = dynamic_cast<_this_class*>(a);

    return (sba != nullptr) && (this->_alloc == sba->_alloc);
}
//------------------------------------------------------------------------------
auto stack_aligned_byte_allocator::has_allocated(
  const owned_block& b,
  span_size_t) noexcept -> tribool {
    return _alloc.has_allocated(b);
}
//------------------------------------------------------------------------------
auto stack_aligned_byte_allocator::allocate(
  size_type n,
  [[maybe_unused]] size_type a) noexcept -> owned_block {
    auto b = _alloc.allocate(n);

    assert(is_aligned_to(b.addr(), a));

    return b;
}
//------------------------------------------------------------------------------
void stack_aligned_byte_allocator::deallocate(
  owned_block&& b,
  [[maybe_unused]] size_type a) noexcept {
    assert(is_aligned_to(b.addr(), a));
    _alloc.deallocate(std::move(b));
}
//------------------------------------------------------------------------------
} // namespace eagine::memory
