/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module eagine.core.math;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.container;

namespace eagine::math {
//------------------------------------------------------------------------------
static auto make_name_to_rgb_map() noexcept {
    flat_map<string_view, vector<float, 3, true>, str_view_less> m;
    const auto add{[&](const string_view name, const char(&str)[8]) {
        m[name] = hex_to_rgb<float>(str);
    }};
    add("AliceBlue", "#f0f8ff");
    add("AntiqueWhite", "#faebd7");
    add("Aqua", "#00ffff");
    add("Aquamarine", "#7fffd4");
    add("Azure", "#f0ffff");
    add("Beige", "#f5f5dc");
    add("Bisque", "#ffe4c4");
    add("Black", "#000000");
    add("BlanchedAlmond", "#ffebcd");
    add("Blue", "#0000ff");
    add("BlueViolet", "#8a2be2");
    add("Brown", "#a52a2a");
    add("BurlyWood", "#deb887");
    add("CadetBlue", "#5f9ea0");
    add("Chartreuse", "#7fff00");
    add("Chocolate", "#d2691e");
    add("Coral", "#ff7f50");
    add("CornflowerBlue", "#6495ed");
    add("Cornsilk", "#fff8dc");
    add("Crimson", "#dc143c");
    add("Cyan", "#00ffff");
    add("DarkBlue", "#00008b");
    add("DarkCyan", "#008b8b");
    add("DarkFern", "#1a4008");
    add("DarkGoldenrod", "#b8860b");
    add("DarkGray", "#a9a9a9");
    add("DarkGreen", "#006400");
    add("DarkKhaki", "#bdb76b");
    add("DarkMagenta", "#8b008b");
    add("DarkOliveGreen", "#556b2f");
    add("DarkOrange", "#ff8c00");
    add("DarkOrchid", "#9932cc");
    add("DarkRed", "#8b0000");
    add("DarkSalmon", "#e9967a");
    add("DarkSeaGreen", "#8fbc8b");
    add("DarkSlateBlue", "#483d8b");
    add("DarkSlateGray", "#2f4f4f");
    add("DarkTurquoise", "#00ced1");
    add("DarkViolet", "#9400d3");
    add("DeepPink", "#ff1493");
    add("DeepSkyBlue", "#00bfff");
    add("DimGray", "#696969");
    add("DodgerBlue", "#1e90ff");
    add("ElectricLime", "#31f718");
    add("FireBrick", "#b22222");
    add("FloralWhite", "#fffaf0");
    add("ForestGreen", "#228b22");
    add("Fuchsia", "#ff00ff");
    add("Gainsboro", "#dcdcdc");
    add("Geraldine", "#f47c7d");
    add("GhostWhite", "#f8f8ff");
    add("Goldenrod", "#daa520");
    add("Gold", "#ffd700");
    add("Gray", "#808080");
    add("Green", "#008000");
    add("GreenYellow", "#adff2f");
    add("HoneyDew", "#f0fff0");
    add("HotPink", "#ff69b4");
    add("IndianRed", "#cd5c5c");
    add("Indigo", "#4b0082");
    add("Ivory", "#fffff0");
    add("Jon", "#3a2124");
    add("Khaki", "#f0e68c");
    add("LavenderBlush", "#fff0f5");
    add("Lavender", "#e6e6fa");
    add("LawnGreen", "#7cfc00");
    add("LemonChiffon", "#fffacd");
    add("LightBlue", "#add8e6");
    add("LightCoral", "#f08080");
    add("LightCyan", "#e0ffff");
    add("LightGoldenrodYellow", "#fafad2");
    add("LightGray", "#d3d3d3");
    add("LightGreen", "#90ee90");
    add("LightPink", "#ffb6c1");
    add("LightSalmon", "#ffa07a");
    add("LightSalmon", "#ffa07a");
    add("LightSeaGreen", "#20b2aa");
    add("LightSkyBlue", "#87cefa");
    add("LightSlateGray", "#778899");
    add("LightSteelBlue", "#b0c4de");
    add("LightYellow", "#ffffe0");
    add("Lime", "#00ff00");
    add("LimeGreen", "#32cd32");
    add("Linen", "#faf0e6");
    add("Magenta", "#ff00ff");
    add("Maroon", "#800000");
    add("MediumAquamarine", "#66cdaa");
    add("MediumBlue", "#0000cd");
    add("MediumOrchid", "#ba55d3");
    add("MediumPurple", "#9370db");
    add("MediumSeaGreen", "#3cb371");
    add("MediumSlateBlue", "#7b68ee");
    add("MediumSlateBlue", "#7b68ee");
    add("MediumSpringGreen", "#00fa9a");
    add("MediumTurquoise", "#48d1cc");
    add("MediumVioletRed", "#c71585");
    add("MidnightBlue", "#191970");
    add("MintCream", "#f5fffa");
    add("MistyRose", "#ffe4e1");
    add("Moccasin", "#ffe4b5");
    add("NavajoWhite", "#ffdead");
    add("Navy", "#000080");
    add("OldLace", "#fdf5e6");
    add("OldRose", "#bb7677");
    add("Olive", "#808000");
    add("OliveDrab", "#6b8e23");
    add("Orange", "#ffa500");
    add("OrangeRed", "#ff4500");
    add("Orchid", "#da70d6");
    add("PaleGoldenrod", "#eee8aa");
    add("PaleGreen", "#98fb98");
    add("PaleTurquoise", "#afeeee");
    add("PaleVioletRed", "#db7093");
    add("PapayaWhip", "#ffefd5");
    add("PeachPuff", "#ffdab9");
    add("Peru", "#cd853f");
    add("Pink", "#ffc0cb");
    add("Plum", "#dda0dd");
    add("PowderBlue", "#b0e0e6");
    add("Purple", "#800080");
    add("RebeccaPurple", "#663399");
    add("Red", "#ff0000");
    add("RosyBrown", "#bc8f8f");
    add("RoyalBlue", "#4169e1");
    add("SaddleBrown", "#8b4513");
    add("Salmon", "#fa8072");
    add("SandyBrown", "#f4a460");
    add("SeaGreen", "#2e8b57");
    add("SeaShell", "#fff5ee");
    add("Shamrock", "#3bc6a4");
    add("Sienna", "#a0522d");
    add("Silver", "#c0c0c0");
    add("SkyBlue", "#87ceeb");
    add("SlateBlue", "#6a5acd");
    add("SlateGray", "#708090");
    add("Snow", "#fffafa");
    add("SpringGreen", "#00ff7f");
    add("SteelBlue", "#4682b4");
    add("Tan", "#d2b48c");
    add("Teal", "#008080");
    add("Thistle", "#d8bfd8");
    add("Tomato", "#ff6347");
    add("Turquoise", "#40e0d0");
    add("Violet", "#ee82ee");
    add("Wheat", "#f5deb3");
    add("Whiskey", "#e5a073");
    add("White", "#ffffff");
    add("WhiteSmoke", "#f5f5f5");
    add("Yellow", "#ffff00");
    add("YellowGreen", "#9acd32");
    return m;
}
//------------------------------------------------------------------------------
auto name_to_rgb(const string_view name) noexcept
  -> optionally_valid<vector<float, 3, true>> {
    static const auto map{make_name_to_rgb_map()};
    return find(map, name);
}
//------------------------------------------------------------------------------
} // namespace eagine::math
