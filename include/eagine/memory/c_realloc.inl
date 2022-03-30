/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include <eagine/assert.hpp>
#include <cstdlib>

namespace eagine::memory {
//------------------------------------------------------------------------------
inline auto c_byte_reallocator::allocate(
  size_type n,
  [[maybe_unused]] size_type a) noexcept -> owned_block {
    EAGINE_ASSERT(a > 0);

    if(n == 0) [[unlikely]] {
        return {};
    }

    // NOLINTNEXTLINE(hicpp-no-malloc,-warnings-as-errors)
    auto p{std::malloc(std_size(n))};

    EAGINE_ASSERT(is_aligned_to(as_address(p), a));

    return this->acquire_block(block(static_cast<byte*>(p), n));
}
//------------------------------------------------------------------------------
inline void c_byte_reallocator::deallocate(owned_block&& b, size_type) noexcept {
    if(!b.empty()) [[likely]] {
        // NOLINTNEXTLINE(hicpp-no-malloc,-warnings-as-errors)
        std::free(b.data());
        this->release_block(std::move(b));
    }
}
//------------------------------------------------------------------------------
inline auto c_byte_reallocator::reallocate(
  owned_block&& b,
  size_type n,
  size_type a) noexcept -> owned_block {
    EAGINE_ASSERT(a > 0);

    if(n == 0) [[unlikely]] {
        deallocate(std::move(b), a);
        return {};
    }

    // NOLINTNEXTLINE(hicpp-no-malloc,-warnings-as-errors)
    auto p{std::realloc(b.data(), std_size(n))};

    this->release_block(std::move(b));

    EAGINE_ASSERT(is_aligned_to(as_address(p), a));

    return this->acquire_block({static_cast<byte*>(p), n});
}
//------------------------------------------------------------------------------
} // namespace eagine::memory
