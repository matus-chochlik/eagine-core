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

import std;
import eagine.core.types;
import eagine.core.concepts;
import :span;
import :address;

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
        if(not b.empty()) [[likely]] {
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
//------------------------------------------------------------------------------
// shared_byte_allocator
//------------------------------------------------------------------------------
export template <typename Base>
class basic_shared_byte_alloc : public Base {
public:
    using value_type = byte;
    using size_type = span_size_t;

    basic_shared_byte_alloc() noexcept = default;

    basic_shared_byte_alloc(const basic_shared_byte_alloc&) noexcept = default;
    basic_shared_byte_alloc(basic_shared_byte_alloc&&) noexcept = default;

    template <does_not_hide<basic_shared_byte_alloc> X>
        requires(std::is_convertible_v<std::decay_t<X>*, byte_allocator*>)
    basic_shared_byte_alloc(X&& x) noexcept
      : _pballoc{std::make_shared<X>(std::forward<X>(x))} {}

    auto operator=(basic_shared_byte_alloc&& that) noexcept
      -> basic_shared_byte_alloc& = default;

    auto operator=(const basic_shared_byte_alloc& that) noexcept
      -> basic_shared_byte_alloc& = default;

    ~basic_shared_byte_alloc() noexcept = default;

    explicit operator bool() const noexcept {
        return bool(_pballoc);
    }

    auto max_size(size_type a) const noexcept -> size_type {
        if(_pballoc) [[likely]] {
            return _pballoc->max_size(a);
        }
        return 0;
    }

    auto has_allocated(const owned_block& b, size_type a) noexcept -> tribool {
        if(_pballoc) [[likely]] {
            return _pballoc->has_allocated(b, a);
        }
        if(b) {
            return false;
        }
        return indeterminate;
    }

    auto allocate(size_type n, size_type a) noexcept -> owned_block {
        if(_pballoc) [[likely]] {
            return _pballoc->allocate(n, a);
        }
        return owned_block{};
    }

    void deallocate(owned_block&& b, size_type a) noexcept {
        if(_pballoc) [[likely]] {
            _pballoc->deallocate(std::move(b), a);
        }
    }

    auto can_reallocate(const owned_block& b, size_type n, size_type a) noexcept
      -> bool {
        if(_pballoc) [[likely]] {
            return _pballoc->can_reallocate(b, n, a);
        }
        return n == b.size();
    }

    auto do_reallocate(owned_block& b, size_type n, size_type a) noexcept
      -> bool {
        if(_pballoc) [[likely]] {
            return _pballoc->do_reallocate(b, n, a);
        }
        return (n == b.size());
    }

    auto reallocate_inplace(owned_block& b, size_type n, size_type a) noexcept
      -> owned_block& {
        if(_pballoc) [[likely]] {
            _pballoc->reallocate_inplace(b, n, a);
        } else {
            assert(n == b.size());
        }
        return b;
    }

    auto reallocate(owned_block&& b, size_type n, size_type a) noexcept
      -> owned_block {
        if(_pballoc) [[likely]] {
            return _pballoc->reallocate(std::move(b), n, a);
        } else {
            assert(n == b.size());
        }
        return std::move(b);
    }

    friend auto operator==(
      const basic_shared_byte_alloc& a,
      const basic_shared_byte_alloc& b) noexcept {
        if((a._pballoc == nullptr) and (b._pballoc == nullptr)) {
            return true;
        } else if(a._pballoc) {
            return a._pballoc->equal(b._pballoc.get());
        }
        return false;
    }

    friend auto operator!=(
      const basic_shared_byte_alloc& a,
      const basic_shared_byte_alloc& b) noexcept {
        return not(a == b);
    }

    template <typename ByteAlloc>
    auto as() -> ByteAlloc& {
        auto* pa = dynamic_cast<ByteAlloc*>(_pballoc);
        if(pa == nullptr) [[unlikely]] {
            throw std::bad_cast();
        }
        return *pa;
    }

private:
    std::shared_ptr<byte_allocator> _pballoc{};
};

export using shared_byte_allocator = basic_shared_byte_alloc<nothing_t>;

export auto default_shared_allocator() -> shared_byte_allocator;
//------------------------------------------------------------------------------
} // namespace eagine::memory
