/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_MEMORY_BUFFER_POOL_HPP
#define EAGINE_MEMORY_BUFFER_POOL_HPP

#include "../config/basic.hpp"
#include "../optional_ref.hpp"
#include "buffer.hpp"
#include <algorithm>
#include <vector>

namespace eagine::memory {
//------------------------------------------------------------------------------
class buffer_pool;
class buffer_pool_stats {
public:
    auto number_of_gets() const noexcept {
        return _gets;
    }

    auto number_of_hits() const noexcept {
        return _hits;
    }

    auto number_of_eats() const noexcept {
        return _eats;
    }

    auto number_of_discards() const noexcept {
        return _dscs;
    }

    auto max_buffer_count() const noexcept {
        return _maxc;
    }

    auto max_buffer_size() const noexcept {
        return _maxs;
    }

private:
    friend class buffer_pool;
    std::uintmax_t _hits{0};
    std::uintmax_t _gets{0};
    std::uintmax_t _eats{0};
    std::uintmax_t _dscs{0};
    std::size_t _maxc{0};
    std::size_t _maxs{0};
};
//------------------------------------------------------------------------------
/// @brief Class storing multiple reusable memory buffer instances.
/// @ingroup memory
/// @see buffer
class buffer_pool {
public:
    /// @brief Default constructors.
    buffer_pool() noexcept = default;

    /// @brief Constructor initializing the maximum number of buffers in the pool.
    explicit buffer_pool(const std::size_t max) noexcept
      : _max{max} {}

    /// @brief Gets a buffer with the specified required size.
    /// @param req_size The returned buffer will have at least this number of bytes.
    /// @see eat
    auto get(const span_size_t req_size = 0) -> memory::buffer {
        memory::buffer result{};
#if !EAGINE_LOW_PROFILE
        _stats._maxc = std::max(_stats._maxc, _pool.size());
        ++_stats._gets;
#endif
        if(!_pool.empty()) [[likely]] {
#if !EAGINE_LOW_PROFILE
            ++_stats._hits;
#endif
            result = std::move(_pool.back());
            _pool.pop_back();
        }
        result.resize(req_size);
        return result;
    }

    /// @brief Returns the specified buffer back to the pool for further reuse.
    /// @see get
    void eat(memory::buffer used) noexcept {
        if(_pool.size() < _max) [[likely]] {
            try {
                _pool.emplace_back(std::move(used));
            } catch(...) {
            }
        } else {
#if !EAGINE_LOW_PROFILE
            ++_stats._dscs;
#endif
        }
#if !EAGINE_LOW_PROFILE
        ++_stats._eats;
        _stats._maxs = std::max(_stats._maxs, std_size(used.capacity()));
#endif
    }

    auto stats() const noexcept
      -> optional_reference_wrapper<const buffer_pool_stats> {
#if !EAGINE_LOW_PROFILE
        return {_stats};
#else
        return {};
#endif
    }

private:
    const std::size_t _max{1024};
    std::vector<memory::buffer> _pool;
#if !EAGINE_LOW_PROFILE
    buffer_pool_stats _stats{};
#endif
};
//------------------------------------------------------------------------------
} // namespace eagine::memory

#endif // EAGINE_MEMORY_BUFFER_POOL_HPP
