/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.utility:count;
import std;
import eagine.core.types;

namespace eagine {

/// @brief Class counting down from the specified number.
/// @ingroup functional
export template <typename Count = unsigned>
class countdown {
public:
    /// @brief Specifies the number of ticks before this countdown completes.
    constexpr countdown(std::integral auto c) noexcept
      : _c{limit_cast<Count>(c)} {}

    /// @brief Indicates if the countdown is completed.
    /// @see tick
    constexpr auto completed() const noexcept -> bool {
        return _c == Count(0);
    }

    /// @brief Indicates if the countdown is not completed.
    /// @see completed
    /// @returns not completed()
    explicit constexpr operator bool() const noexcept {
        return not completed();
    }

    /// @brief Decreases the count on this countdown.
    /// @see completed
    /// @see pre not completed()
    constexpr auto tick() noexcept -> countdown& {
        assert(not completed());
        _c--;
        return *this;
    }

private:
    Count _c;
};

template <std::integral Int>
countdown(Int) -> countdown<std::make_unsigned_t<Int>>;

} // namespace eagine

