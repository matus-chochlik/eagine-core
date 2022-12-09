/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.memory:byte_allocator;

import eagine.core.types;
import :block;
import :address;
import <cstdint>;
import <cstdlib>;
import <limits>;
import <utility>;

namespace eagine::memory {
//------------------------------------------------------------------------------
// byte_allocator
export struct byte_allocator
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

    virtual auto can_reallocate(
      [[maybe_unused]] const owned_block& b,
      [[maybe_unused]] size_type n,
      [[maybe_unused]] size_type a) noexcept -> bool {
        return false;
    }

    virtual auto do_reallocate(
      [[maybe_unused]] owned_block& b,
      [[maybe_unused]] size_type n,
      [[maybe_unused]] size_type a) noexcept -> bool {
        if(can_reallocate(b, n, a)) {
            b = reallocate(std::move(b), n, a);
            return b.size() == n;
        }
        return false;
    }

    auto reallocate_inplace(owned_block& b, size_type n, size_type a) noexcept
      -> owned_block& {
        if(b.size() != n) {
            if(can_reallocate(b, n, a)) {
                do_reallocate(b, n, a);
            } else {
                deallocate(std::move(b), a);
                b = allocate(n, a);
            }
        }
        return b;
    }

    virtual auto reallocate(owned_block&& b, size_type n, size_type a) noexcept
      -> owned_block {
        return std::move(reallocate_inplace(b, n, a));
    }
};
//------------------------------------------------------------------------------
export class c_byte_reallocator final : public byte_allocator {
public:
    using size_type = span_size_t;

    auto equal(byte_allocator* a) const noexcept -> bool final {
        return dynamic_cast<c_byte_reallocator*>(a) != nullptr;
    }

    auto max_size(size_type) noexcept -> size_type final {
        return std::numeric_limits<size_type>::max();
    }

    auto has_allocated(const owned_block&, size_type) noexcept
      -> tribool final {
        return indeterminate;
    }

    auto allocate(size_type n, [[maybe_unused]] size_type a) noexcept
      -> owned_block final {
        assert(a > 0);

        if(n == 0) [[unlikely]] {
            return {};
        }

        // NOLINTNEXTLINE(hicpp-no-malloc,-warnings-as-errors)
        auto p{std::malloc(std_size(n))};

        assert(is_aligned_to(as_address(p), a));

        return this->acquire_block(block(static_cast<byte*>(p), n));
    }

    void deallocate(owned_block&& b, size_type) noexcept final {
        if(!b.empty()) [[likely]] {
            // NOLINTNEXTLINE(hicpp-no-malloc,-warnings-as-errors)
            std::free(b.data());
            this->release_block(std::move(b));
        }
    }

    auto can_reallocate(const owned_block&, size_type, size_type) noexcept
      -> bool final {
        return true;
    }

    auto reallocate(owned_block&& b, size_type n, size_type a) noexcept
      -> owned_block final {
        assert(a > 0);

        if(n == 0) [[unlikely]] {
            deallocate(std::move(b), a);
            return {};
        }

        // NOLINTNEXTLINE(hicpp-no-malloc,-warnings-as-errors)
        auto p{std::realloc(b.data(), std_size(n))};

        this->release_block(std::move(b));

        assert(is_aligned_to(as_address(p), a));

        return this->acquire_block({static_cast<byte*>(p), n});
    }
};
//------------------------------------------------------------------------------
export using default_byte_allocator = c_byte_reallocator;

} // namespace eagine::memory
