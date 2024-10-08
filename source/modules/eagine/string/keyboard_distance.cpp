/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.string:keyboard_distance;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.math;
import eagine.core.container;

namespace eagine {

export class default_keyboard_layout {
public:
    using coord_type = math::vector<float, 3, true>;

    default_keyboard_layout(const float shift = 1.F);

    [[nodiscard]] auto get_coord(const char c) noexcept -> coord_type {
        return find(_key_coord, c).value_or(coord_type{6.F, 3.F, 0.5F});
    }

    template <typename Function>
    [[nodiscard]] auto for_each_char_coord(Function function) const noexcept {
        for(const auto& [kchr, coord] : _key_coord) {
            function(kchr, coord);
        }
    }

protected:
    void add_key_coord(
      const char key,
      const math::vector<float, 3, true> coord) {
        _key_coord[key] = coord;
    }

private:
    flat_map<char, math::vector<float, 3, true>> _key_coord;
};
//------------------------------------------------------------------------------
export class keyboard_distance {
public:
    template <typename Layout>
    keyboard_distance(const Layout& layout, const float multiplier = 1.F) {
        layout.for_each_char_coord([&](char lc, auto lp) {
            layout.for_each_char_coord([&](char rc, auto rp) {
                if(lc != rc) {
                    _key_dist[std::pair<char, char>{lc, rc}] =
                      multiplier * math::distance(lp, rp);
                }
            });
        });
    }

    [[nodiscard]] auto operator()(const string_view ls, const string_view rs)
      const -> float;

private:
    flat_map<std::pair<char, char>, float> _key_dist;
};

} // namespace eagine
