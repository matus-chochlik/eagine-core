/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.utility:progress_bar;

import eagine.core.types;
import eagine.core.memory;
import <string>;

namespace eagine {
//------------------------------------------------------------------------------
export class progress_bar {
public:
    progress_bar() noexcept = default;

    auto set_width(const int w) noexcept -> auto& {
        _width = w < 0 ? 0 : w;
        return *this;
    }

    auto set_min(const float min) noexcept -> auto& {
        _min = min;
        return *this;
    }

    auto set_max(const float max) noexcept -> auto& {
        _max = max;
        return *this;
    }

    auto reformat() noexcept -> progress_bar&;

    auto update(const float value) noexcept -> progress_bar&;

    auto get() const noexcept -> string_view {
        return {_buffer.c_str(), span_size_t(_idx)};
    }

private:
    int _width{80};
    float _min{-1.F};
    float _max{+1.F};
    float _prev{0.F};
    float _curr{0.F};
    std::size_t _idx{0};
    std::string _buffer;
};
//------------------------------------------------------------------------------
} // namespace eagine
