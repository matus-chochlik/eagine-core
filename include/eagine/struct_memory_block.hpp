/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_STRUCT_MEMORY_BLOCK_HPP
#define EAGINE_STRUCT_MEMORY_BLOCK_HPP

#include "assert.hpp"
#include "memory/block.hpp"
#include <type_traits>

namespace eagine {

template <typename T>
class structured_memory_block {
private:
    memory::basic_block<std::is_const_v<T>> _blk;

    template <typename X = T>
    auto _ptr() noexcept
        requires(!std::is_const_v<X>)
    {
        EAGINE_ASSERT(is_valid_block(_blk));
        return static_cast<X*>(_blk.addr());
    }

    auto _cptr() const noexcept {
        EAGINE_ASSERT(is_valid_block(_blk));
        return static_cast<const T*>(_blk.addr());
    }

public:
    static auto is_valid_block(const memory::const_block blk) noexcept -> bool {
        return !blk.empty() && (blk.is_aligned_as<T>()) &&
               (can_accommodate(blk, std::type_identity<T>()));
    }

    structured_memory_block(memory::basic_block<std::is_const_v<T>> blk) noexcept
      : _blk(blk) {
        EAGINE_ASSERT(is_valid_block(_blk));
    }

    template <typename X = T>
    auto get() noexcept
      -> X& requires(!std::is_const_v<X> && std::is_same_v<X, T>) {
                return *_ptr();
            }

    template <typename X = T>
    auto operator->() noexcept
      -> X* requires(!std::is_const_v<X> && std::is_same_v<X, T>) {
                return _ptr();
            }

    auto get() const noexcept -> const T& {
        return *_cptr();
    }

    auto operator->() const noexcept -> const T* {
        return _cptr();
    }
};

} // namespace eagine

#endif // EAGINE_STRUCT_MEMORY_BLOCK_HPP
