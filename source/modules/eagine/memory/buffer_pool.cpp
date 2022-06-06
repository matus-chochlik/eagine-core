/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.memory:buffer_pool;

import eagine.core.build_config;
import eagine.core.types;
export import :buffer;
import <algorithm>;
import <vector>;

namespace eagine::memory {
//------------------------------------------------------------------------------
export class buffer_pool;
export class buffer_pool_stats {
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
export class buffer_pool {
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
        if constexpr(!low_profile_build) {
            _stats._maxc = std::max(_stats._maxc, _pool.size());
            ++_stats._gets;
        }
        if(!_pool.empty()) [[likely]] {
            if constexpr(!low_profile_build) {
                ++_stats._hits;
            }
            result = std::move(_pool.back());
            _pool.pop_back();
        }
        result.resize(req_size);
        return result;
    }

    /// @brief Returns the specified buffer back to the pool for further reuse.
    /// @see get
    void eat(memory::buffer used) noexcept {
        const auto old_cap{std_size(used.capacity())};
        if(_pool.size() < _max) [[likely]] {
            try {
                _pool.emplace_back(std::move(used));
            } catch(...) {
            }
        } else {
            if constexpr(!low_profile_build) {
                ++_stats._dscs;
            }
        }
        if constexpr(!low_profile_build) {
            ++_stats._eats;
            _stats._maxs = std::max(_stats._maxs, old_cap);
        }
    }

    auto stats() const noexcept
      -> optional_reference_wrapper<const buffer_pool_stats> {
        if constexpr(!low_profile_build) {
            return {_stats};
        } else {
            return {nothing};
        }
    }

private:
    const std::size_t _max{1024};
    std::vector<memory::buffer> _pool;
    [[no_unique_address]] not_in_low_profile<buffer_pool_stats> _stats{};
};
//------------------------------------------------------------------------------
} // namespace eagine::memory
