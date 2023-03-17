/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.memory:shared_allocator;

import eagine.core.types;
import eagine.core.concepts;
import :byte_allocator;
import :block;
import std;

namespace eagine::memory {

// shared_byte_allocator
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

} // namespace eagine::memory
