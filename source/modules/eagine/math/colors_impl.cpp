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
    const auto add{[&](const char(&str)[8], const string_view name) {
        m[name] = hex_to_rgb<float>(str);
    }};
    add("#f0f8ff", "AliceBlue");
    add("#faebd7", "AntiqueWhite");
    add("#00ffff", "Aqua");
    add("#7fffd4", "Aquamarine");
    add("#f0ffff", "Azure");
    add("#f5f5dc", "Beige");
    add("#ffe4c4", "Bisque");
    add("#000000", "Black");
    add("#ffebcd", "BlanchedAlmond");
    add("#0000ff", "Blue");
    add("#8a2be2", "BlueViolet");
    add("#a52a2a", "Brown");
    add("#deb887", "BurlyWood");
    add("#5f9ea0", "CadetBlue");
    add("#7fff00", "Chartreuse");
    add("#d2691e", "Chocolate");
    add("#ff7f50", "Coral");
    add("#6495ed", "CornflowerBlue");
    add("#fff8dc", "Cornsilk");
    add("#dc143c", "Crimson");
    add("#00ffff", "Cyan");
    add("#00008b", "DarkBlue");
    add("#008b8b", "DarkCyan");
    add("#1a4008", "DarkFern");
    add("#b8860b", "DarkGoldenrod");
    add("#a9a9a9", "DarkGray");
    add("#006400", "DarkGreen");
    add("#bdb76b", "DarkKhaki");
    add("#8b008b", "DarkMagenta");
    add("#556b2f", "DarkOliveGreen");
    add("#ff8c00", "DarkOrange");
    add("#9932cc", "DarkOrchid");
    add("#8b0000", "DarkRed");
    add("#e9967a", "DarkSalmon");
    add("#8fbc8b", "DarkSeaGreen");
    add("#483d8b", "DarkSlateBlue");
    add("#2f4f4f", "DarkSlateGray");
    add("#00ced1", "DarkTurquoise");
    add("#9400d3", "DarkViolet");
    add("#ff1493", "DeepPink");
    add("#00bfff", "DeepSkyBlue");
    add("#696969", "DimGray");
    add("#1e90ff", "DodgerBlue");
    add("#31f718", "ElectricLime");
    add("#b22222", "FireBrick");
    add("#fffaf0", "FloralWhite");
    add("#228b22", "ForestGreen");
    add("#ff00ff", "Fuchsia");
    add("#dcdcdc", "Gainsboro");
    add("#f47c7d", "Geraldine");
    add("#f8f8ff", "GhostWhite");
    add("#daa520", "Goldenrod");
    add("#ffd700", "Gold");
    add("#808080", "Gray");
    add("#008000", "Green");
    add("#adff2f", "GreenYellow");
    add("#f0fff0", "HoneyDew");
    add("#ff69b4", "HotPink");
    add("#cd5c5c", "IndianRed");
    add("#4b0082", "Indigo");
    add("#fffff0", "Ivory");
    add("#3a2124", "Jon");
    add("#f0e68c", "Khaki");
    add("#fff0f5", "LavenderBlush");
    add("#e6e6fa", "Lavender");
    add("#7cfc00", "LawnGreen");
    add("#fffacd", "LemonChiffon");
    add("#add8e6", "LightBlue");
    add("#f08080", "LightCoral");
    add("#e0ffff", "LightCyan");
    add("#fafad2", "LightGoldenrodYellow");
    add("#d3d3d3", "LightGray");
    add("#90ee90", "LightGreen");
    add("#ffb6c1", "LightPink");
    add("#ffa07a", "LightSalmon");
    add("#20b2aa", "LightSeaGreen");
    add("#87cefa", "LightSkyBlue");
    add("#778899", "LightSlateGray");
    add("#b0c4de", "LightSteelBlue");
    add("#ffffe0", "LightYellow");
    add("#00ff00", "Lime");
    add("#32cd32", "LimeGreen");
    add("#faf0e6", "Linen");
    add("#ff00ff", "Magenta");
    add("#800000", "Maroon");
    add("#66cdaa", "MediumAquamarine");
    add("#0000cd", "MediumBlue");
    add("#ba55d3", "MediumOrchid");
    add("#9370db", "MediumPurple");
    add("#3cb371", "MediumSeaGreen");
    add("#7b68ee", "MediumSlateBlue");
    add("#7b68ee", "MediumSlateBlue");
    add("#00fa9a", "MediumSpringGreen");
    add("#48d1cc", "MediumTurquoise");
    add("#c71585", "MediumVioletRed");
    add("#191970", "MidnightBlue");
    add("#f5fffa", "MintCream");
    add("#ffe4e1", "MistyRose");
    add("#ffe4b5", "Moccasin");
    add("#ffdead", "NavajoWhite");
    add("#000080", "Navy");
    add("#fdf5e6", "OldLace");
    add("#bb7677", "OldRose");
    add("#808000", "Olive");
    add("#6b8e23", "OliveDrab");
    add("#ffa500", "Orange");
    add("#ff4500", "OrangeRed");
    add("#da70d6", "Orchid");
    add("#eee8aa", "PaleGoldenrod");
    add("#98fb98", "PaleGreen");
    add("#afeeee", "PaleTurquoise");
    add("#db7093", "PaleVioletRed");
    add("#ffefd5", "PapayaWhip");
    add("#ffdab9", "PeachPuff");
    add("#cd853f", "Peru");
    add("#ffc0cb", "Pink");
    add("#dda0dd", "Plum");
    add("#b0e0e6", "PowderBlue");
    add("#800080", "Purple");
    add("#663399", "RebeccaPurple");
    add("#ff0000", "Red");
    add("#bc8f8f", "RosyBrown");
    add("#4169e1", "RoyalBlue");
    add("#8b4513", "SaddleBrown");
    add("#fa8072", "Salmon");
    add("#f4a460", "SandyBrown");
    add("#2e8b57", "SeaGreen");
    add("#fff5ee", "SeaShell");
    add("#3bc6a4", "Shamrock");
    add("#a0522d", "Sienna");
    add("#c0c0c0", "Silver");
    add("#87ceeb", "SkyBlue");
    add("#6a5acd", "SlateBlue");
    add("#708090", "SlateGray");
    add("#fffafa", "Snow");
    add("#00ff7f", "SpringGreen");
    add("#4682b4", "SteelBlue");
    add("#d2b48c", "Tan");
    add("#008080", "Teal");
    add("#d8bfd8", "Thistle");
    add("#ff6347", "Tomato");
    add("#40e0d0", "Turquoise");
    add("#ee82ee", "Violet");
    add("#f5deb3", "Wheat");
    add("#e5a073", "Whiskey");
    add("#ffffff", "White");
    add("#f5f5f5", "WhiteSmoke");
    add("#ffff00", "Yellow");
    add("#9acd32", "YellowGreen");
    return m;
}
//------------------------------------------------------------------------------
auto name_to_rgb(const string_view name) noexcept
  -> optionally_valid<vector<float, 3, true>> {
    static const auto map{make_name_to_rgb_map()};
    return find(map, name);
}
//------------------------------------------------------------------------------
static auto color_float_to_int(math::vector<float, 3, true> c) noexcept
  -> optionally_valid<vector<int, 3, true>> {
    return {{int(c.x() * 255), int(c.y() * 255), int(c.z() * 255)}};
}
//------------------------------------------------------------------------------
auto name_to_rgb_int(const string_view name) noexcept
  -> optionally_valid<vector<int, 3, true>> {
    return name_to_rgb(name).and_then(color_float_to_int);
}
//------------------------------------------------------------------------------
} // namespace eagine::math
