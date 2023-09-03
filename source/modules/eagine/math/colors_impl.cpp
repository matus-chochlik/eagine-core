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
    const auto c{[](const char(&str)[8]) {
        return hex_to_rgb<float>(str);
    }};
    flat_map<string_view, vector<float, 3, true>, str_view_less> m;
    m["AliceBlue"] = c("#f0f8ff");
    m["AntiqueWhite"] = c("#faebd7");
    m["Aqua"] = c("#00ffff");
    m["Aquamarine"] = c("#7fffd4");
    m["Azure"] = c("#f0ffff");
    m["Beige"] = c("#f5f5dc");
    m["Bisque"] = c("#ffe4c4");
    m["Black"] = c("#000000");
    m["BlanchedAlmond"] = c("#ffebcd");
    m["Blue"] = c("#0000ff");
    m["BlueViolet"] = c("#8a2be2");
    m["Brown"] = c("#a52a2a");
    m["BurlyWood"] = c("#deb887");
    m["CadetBlue"] = c("#5f9ea0");
    m["Chartreuse"] = c("#7fff00");
    m["Chocolate"] = c("#d2691e");
    m["Coral"] = c("#ff7f50");
    m["CornflowerBlue"] = c("#6495ed");
    m["Cornsilk"] = c("#fff8dc");
    m["Crimson"] = c("#dc143c");
    m["Cyan"] = c("#00ffff");
    m["DarkBlue"] = c("#00008b");
    m["DarkCyan"] = c("#008b8b");
    m["DarkGoldenrod"] = c("#b8860b");
    m["DarkGray"] = c("#a9a9a9");
    m["DarkGreen"] = c("#006400");
    m["DarkKhaki"] = c("#bdb76b");
    m["DarkMagenta"] = c("#8b008b");
    m["DarkOliveGreen"] = c("#556b2f");
    m["DarkOrange"] = c("#ff8c00");
    m["DarkOrchid"] = c("#9932cc");
    m["DarkRed"] = c("#8b0000");
    m["DarkSalmon"] = c("#e9967a");
    m["DarkSeaGreen"] = c("#8fbc8b");
    m["DarkSlateBlue"] = c("#483d8b");
    m["DarkSlateGray"] = c("#2f4f4f");
    m["DarkTurquoise"] = c("#00ced1");
    m["DarkViolet"] = c("#9400d3");
    m["DeepPink"] = c("#ff1493");
    m["DeepSkyBlue"] = c("#00bfff");
    m["DimGray"] = c("#696969");
    m["DodgerBlue"] = c("#1e90ff");
    m["FireBrick"] = c("#b22222");
    m["FloralWhite"] = c("#fffaf0");
    m["ForestGreen"] = c("#228b22");
    m["Fuchsia"] = c("#ff00ff");
    m["Gainsboro"] = c("#dcdcdc");
    m["Geraldine"] = c("#f47c7d");
    m["GhostWhite"] = c("#f8f8ff");
    m["Goldenrod"] = c("#daa520");
    m["Gold"] = c("#ffd700");
    m["Gray"] = c("#808080");
    m["Green"] = c("#008000");
    m["GreenYellow"] = c("#adff2f");
    m["HoneyDew"] = c("#f0fff0");
    m["HotPink"] = c("#ff69b4");
    m["IndianRed"] = c("#cd5c5c");
    m["Indigo"] = c("#4b0082");
    m["Ivory"] = c("#fffff0");
    m["Jon"] = c("#3a2124");
    m["Khaki"] = c("#f0e68c");
    m["LavenderBlush"] = c("#fff0f5");
    m["Lavender"] = c("#e6e6fa");
    m["LawnGreen"] = c("#7cfc00");
    m["LemonChiffon"] = c("#fffacd");
    m["LightBlue"] = c("#add8e6");
    m["LightCoral"] = c("#f08080");
    m["LightCyan"] = c("#e0ffff");
    m["LightGoldenrodYellow"] = c("#fafad2");
    m["LightGray"] = c("#d3d3d3");
    m["LightGreen"] = c("#90ee90");
    m["LightPink"] = c("#ffb6c1");
    m["LightSalmon"] = c("#ffa07a");
    m["LightSalmon"] = c("#ffa07a");
    m["LightSeaGreen"] = c("#20b2aa");
    m["LightSkyBlue"] = c("#87cefa");
    m["LightSlateGray"] = c("#778899");
    m["LightSteelBlue"] = c("#b0c4de");
    m["LightYellow"] = c("#ffffe0");
    m["Lime"] = c("#00ff00");
    m["LimeGreen"] = c("#32cd32");
    m["Linen"] = c("#faf0e6");
    m["Magenta"] = c("#ff00ff");
    m["Maroon"] = c("#800000");
    m["MediumAquamarine"] = c("#66cdaa");
    m["MediumBlue"] = c("#0000cd");
    m["MediumOrchid"] = c("#ba55d3");
    m["MediumPurple"] = c("#9370db");
    m["MediumSeaGreen"] = c("#3cb371");
    m["MediumSlateBlue"] = c("#7b68ee");
    m["MediumSlateBlue"] = c("#7b68ee");
    m["MediumSpringGreen"] = c("#00fa9a");
    m["MediumTurquoise"] = c("#48d1cc");
    m["MediumVioletRed"] = c("#c71585");
    m["MidnightBlue"] = c("#191970");
    m["MintCream"] = c("#f5fffa");
    m["MistyRose"] = c("#ffe4e1");
    m["Moccasin"] = c("#ffe4b5");
    m["NavajoWhite"] = c("#ffdead");
    m["Navy"] = c("#000080");
    m["OldLace"] = c("#fdf5e6");
    m["Olive"] = c("#808000");
    m["OliveDrab"] = c("#6b8e23");
    m["Orange"] = c("#ffa500");
    m["OrangeRed"] = c("#ff4500");
    m["Orchid"] = c("#da70d6");
    m["PaleGoldenrod"] = c("#eee8aa");
    m["PaleGreen"] = c("#98fb98");
    m["PaleTurquoise"] = c("#afeeee");
    m["PaleVioletRed"] = c("#db7093");
    m["PapayaWhip"] = c("#ffefd5");
    m["PeachPuff"] = c("#ffdab9");
    m["Peru"] = c("#cd853f");
    m["Pink"] = c("#ffc0cb");
    m["Plum"] = c("#dda0dd");
    m["PowderBlue"] = c("#b0e0e6");
    m["Purple"] = c("#800080");
    m["RebeccaPurple"] = c("#663399");
    m["Red"] = c("#ff0000");
    m["RosyBrown"] = c("#bc8f8f");
    m["RoyalBlue"] = c("#4169e1");
    m["SaddleBrown"] = c("#8b4513");
    m["Salmon"] = c("#fa8072");
    m["SandyBrown"] = c("#f4a460");
    m["SeaGreen"] = c("#2e8b57");
    m["SeaShell"] = c("#fff5ee");
    m["Shamrock"] = c("#3bc6a4");
    m["Sienna"] = c("#a0522d");
    m["Silver"] = c("#c0c0c0");
    m["SkyBlue"] = c("#87ceeb");
    m["SlateBlue"] = c("#6a5acd");
    m["SlateGray"] = c("#708090");
    m["Snow"] = c("#fffafa");
    m["SpringGreen"] = c("#00ff7f");
    m["SteelBlue"] = c("#4682b4");
    m["Tan"] = c("#d2b48c");
    m["Teal"] = c("#008080");
    m["Thistle"] = c("#d8bfd8");
    m["Tomato"] = c("#ff6347");
    m["Turquoise"] = c("#40e0d0");
    m["Violet"] = c("#ee82ee");
    m["Wheat"] = c("#f5deb3");
    m["White"] = c("#ffffff");
    m["WhiteSmoke"] = c("#f5f5f5");
    m["Yellow"] = c("#ffff00");
    m["YellowGreen"] = c("#9acd32");
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
