/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.utility:sliding_average;

import <array>;
import <cstdint>;
import <numeric>;

namespace eagine {
//------------------------------------------------------------------------------
export template <
  typename T,
  typename D,
  std::size_t numSlots,
  std::size_t numSlotValues>
class basic_sliding_average {
public:
    constexpr basic_sliding_average() noexcept = default;

    auto add(T value) noexcept -> basic_sliding_average& {
        if(++_counts[_current] >= numSlotValues) {
            _current = (_current + 1) % numSlots;
            _counts[_current] = 1;
            _values[_current] = value;
        } else {
            _values[_current] += value;
        }
        return *this;
    }

    auto get_sum() const noexcept -> T {
        return std::accumulate(_values.begin(), _values.end(), T{});
    }

    auto get_count() const noexcept -> D {
        return D(std::accumulate(_counts.begin(), _counts.end(), 0Z));
    }

    auto get() const noexcept -> T {
        if(const auto div{get_count()}; div != 0) {
            return get_sum() / div;
        }
        return T{};
    }

private:
    std::array<T, numSlots> _values{};
    std::array<std::size_t, numSlots> _counts{};
    std::size_t _current{0Z};
};
//------------------------------------------------------------------------------
} // namespace eagine

