/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_MEMORY_BYTE_ALLOC_HPP
#define EAGINE_MEMORY_BYTE_ALLOC_HPP

#include "../assert.hpp"
#include "../interface.hpp"
#include "../tribool.hpp"
#include "../types.hpp"
#include "block.hpp"
#include <cstdint>

namespace eagine::memory {

// byte_allocator
struct byte_allocator
  : interface<byte_allocator>
  , block_owner {
    using value_type = byte;
    using size_type = span_size_t;

    virtual auto equal(byte_allocator* a) const noexcept -> bool = 0;

    virtual auto max_size(size_type a) noexcept -> size_type = 0;

    virtual auto has_allocated(const owned_block& b, size_type a = 0) noexcept
      -> tribool = 0;

    virtual auto allocate(size_type n, size_type a) noexcept -> owned_block = 0;

    virtual void deallocate(owned_block&& b, size_type a = 0) noexcept = 0;

    virtual auto
    can_reallocate(const owned_block& b, size_type n, size_type a) noexcept
      -> bool {
        EAGINE_MAYBE_UNUSED(b);
        EAGINE_MAYBE_UNUSED(n);
        EAGINE_MAYBE_UNUSED(a);
        return false;
    }

    virtual auto reallocate(owned_block&& b, size_type n, size_type a) noexcept
      -> owned_block {
        EAGINE_MAYBE_UNUSED(b);
        EAGINE_MAYBE_UNUSED(n);
        EAGINE_MAYBE_UNUSED(a);
        return {};
    }

    void do_reallocate(owned_block& b, size_type n, size_type a) noexcept {
        if(b.size() != n) {
            if(can_reallocate(b, n, a)) {
                b = reallocate(std::move(b), n, a);
            } else {
                deallocate(std::move(b), a);
                b = allocate(n, a);
            }
        }
    }
};

} // namespace eagine::memory

#endif // EAGINE_MEMORY_BYTE_ALLOC_HPP
