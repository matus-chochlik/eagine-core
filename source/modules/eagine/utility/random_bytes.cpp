/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.utility:random_bytes;

import eagine.core.types;
import eagine.core.memory;
import <climits>;
import <random>;

namespace eagine {

export template <typename Engine>
auto fill_with_random_bytes(span<byte> dst, Engine& engine) -> span<byte> {
    using ui_t = typename Engine::result_type;

    const ui_t mask = ((1U << unsigned(CHAR_BIT)) - 1U);
    std::independent_bits_engine<Engine, CHAR_BIT, ui_t> ibe(engine);

    generate(dst, [&] { return static_cast<byte>(ibe() & mask); });
    return dst;
}

export auto fill_with_random_bytes(span<byte> dst) -> span<byte> {
    std::random_device rd;
    std::default_random_engine re{rd()};
    return fill_with_random_bytes(dst, re);
}

} // namespace eagine
