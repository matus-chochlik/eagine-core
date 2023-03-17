/// @example eagine/edit_distance.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
import eagine.core;
import std;

namespace eagine {

auto main(main_ctx& ctx) -> int {

    const identifier cioid{identifier{"words"}};
    std::ifstream input("/usr/share/dict/words");
    std::vector<std::string> words;
    std::string word;

    while(std::getline(input, word).good()) {
        words.emplace_back(std::move(word));
    }

    const keyboard_distance kb_dist{default_keyboard_layout{}, 0.5F};

    while(std::getline(std::cin, word).good()) {
        std::size_t min_idx = words.size();
        auto min_dist = float(word.length() + 1);
        for(const auto idx : integer_range(words.size())) {
            const auto dist = kb_dist(view(word), view(words[idx]));
            if(min_dist > dist) {
                min_dist = dist;
                min_idx = idx;
            }
        }
        if(min_idx < words.size()) {
            if(are_equal(min_dist, 0.F)) {
                ctx.cio()
                  .print(cioid, "found: ${word}")
                  .arg(identifier{"word"}, words[min_idx]);
            } else {
                ctx.cio()
                  .print(cioid, "did you mean ${word}?")
                  .arg(identifier{"word"}, words[min_idx]);
            }
        } else {
            ctx.cio()
              .print(cioid, "did not find anything similar to ${word}")
              .arg(identifier{"word"}, word);
        }
    }

    return 0;
}

} // namespace eagine

auto main(int argc, const char** argv) -> int {
    return eagine::default_main(argc, argv, eagine::main);
}

