/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.memory:stack_allocator;

import eagine.core.types;
import :block;
import :address;
import :byte_allocator;
import <type_traits>;

namespace eagine::memory {
//------------------------------------------------------------------------------
// base_stack_allocator
// non-rebindable non-copyable stack allocator
// use with care!
//------------------------------------------------------------------------------
export template <typename T>
class base_stack_allocator : public block_owner {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = span_size_t;
    using difference_type = std::ptrdiff_t;

    base_stack_allocator(base_stack_allocator&& tmp) noexcept;
    base_stack_allocator(const base_stack_allocator&) = delete;
    auto operator=(base_stack_allocator&& tmp) = delete;
    auto operator=(const base_stack_allocator&) = delete;

    base_stack_allocator() noexcept = default;

    base_stack_allocator(const block& blk, span_size_t align) noexcept;

    base_stack_allocator(const block& blk) noexcept;

    ~base_stack_allocator() noexcept;

    auto max_size() const noexcept -> size_type {
        return _available().size();
    }

    auto allocated() const noexcept -> const_block {
        return _allocated();
    }

    auto allocated_size() const noexcept -> size_type {
        return _allocated().size();
    }

    auto contains(const owned_block& b) const noexcept -> bool;
    auto has_allocated(const owned_block& b) const noexcept -> tribool;

    auto allocate(size_type n) noexcept -> owned_block;

    auto truncate(owned_block&& b, size_type nn) noexcept -> owned_block;

    void deallocate(owned_block&& b) noexcept;

    friend auto operator==(
      const base_stack_allocator& a,
      const base_stack_allocator& b) noexcept {
        if((a._btm == b._btm) && (a._top == b._top)) {
            assert(a._pos == b._pos);
            assert(a._min == b._min);
            assert(a._dif == b._dif);

            return true;
        }
        return false;
    }

private:
    T* _btm{nullptr};
    T* _top{nullptr};
    T* _pos{nullptr};
    T* _min{nullptr};
    span_size_t _dif{0};

    auto _store() const noexcept -> const_block;
    auto _allocated() const noexcept -> const_block;
    auto _available() const noexcept -> const_block;
};
//------------------------------------------------------------------------------
// stack_byte_allocator_only
//------------------------------------------------------------------------------
export class stack_byte_allocator_only : public byte_allocator {
public:
    using value_type = byte;
    using size_type = span_size_t;

    stack_byte_allocator_only(stack_byte_allocator_only&&) noexcept = default;
    stack_byte_allocator_only(const stack_byte_allocator_only&) = delete;
    auto operator=(stack_byte_allocator_only&&) = delete;
    auto operator=(const stack_byte_allocator_only&) = delete;

    ~stack_byte_allocator_only() noexcept override = default;

    stack_byte_allocator_only(const block& blk)
      : _alloc{blk} {}

    auto equal(byte_allocator* a) const noexcept -> bool override;

    auto max_size(size_type) noexcept -> size_type override {
        return _alloc.max_size();
    }

    auto allocated() const noexcept -> const_block {
        return _alloc.allocated();
    }

    auto has_allocated(const owned_block& b, span_size_t) noexcept
      -> tribool override {
        return _alloc.has_allocated(b);
    }

    auto allocate(size_type n, size_type a) noexcept -> owned_block override;

    void deallocate(owned_block&& b, size_type) noexcept override;

private:
    base_stack_allocator<byte> _alloc;
};
//------------------------------------------------------------------------------
// stack_byte_allocator
//------------------------------------------------------------------------------
export class stack_byte_allocator final : public byte_allocator {

public:
    using value_type = byte;
    using size_type = span_size_t;

    stack_byte_allocator(stack_byte_allocator&&) noexcept = default;
    stack_byte_allocator(const stack_byte_allocator&) = delete;
    auto operator=(stack_byte_allocator&&) = delete;
    auto operator=(const stack_byte_allocator&) = delete;
    ~stack_byte_allocator() noexcept final = default;

    stack_byte_allocator(const block& blk)
      : _alloc{blk} {}

    auto equal(byte_allocator* a) const noexcept -> bool override;

    auto max_size(size_type a) noexcept -> size_type override {
        return _alloc.max_size() > a ? _alloc.max_size() - a : 0;
    }

    auto has_allocated(const owned_block& b, span_size_t) noexcept
      -> tribool override {
        return _alloc.has_allocated(b);
    }

    auto allocate(size_type n, size_type a) noexcept -> owned_block override;

    void deallocate(owned_block&& b, size_type) noexcept override;

private:
    base_stack_allocator<byte> _alloc;
};
//------------------------------------------------------------------------------
// stack_aligned_byte_allocator
//------------------------------------------------------------------------------
export class stack_aligned_byte_allocator final : public byte_allocator {
    using _this_class = stack_aligned_byte_allocator;

public:
    using value_type = byte;
    using size_type = span_size_t;

    stack_aligned_byte_allocator(_this_class&&) noexcept = default;
    stack_aligned_byte_allocator(const _this_class&) = delete;
    auto operator=(stack_aligned_byte_allocator&&) = delete;
    auto operator=(const stack_aligned_byte_allocator&) = delete;
    ~stack_aligned_byte_allocator() noexcept final = default;

    stack_aligned_byte_allocator(const block& blk, span_size_t align)
      : _align{align}
      , _alloc{blk, _align} {}

    auto equal(byte_allocator* a) const noexcept -> bool override;

    auto max_size(size_type) noexcept -> size_type override {
        return _alloc.max_size();
    }

    auto has_allocated(const owned_block& b, span_size_t) noexcept
      -> tribool override;

    auto allocate(size_type n, size_type a) noexcept -> owned_block override;

    void deallocate(owned_block&& b, size_type a) noexcept override;

private:
    span_size_t _align;

    base_stack_allocator<byte> _alloc;
};
//------------------------------------------------------------------------------
} // namespace eagine::memory

