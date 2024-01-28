/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module eagine.core.string;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.utility;

namespace eagine {
//------------------------------------------------------------------------------
template <typename Putter>
void _hexdump_to_hex_b(Putter& put_char, byte b) {
    static const char hd[16] = {
      '0',
      '1',
      '2',
      '3',
      '4',
      '5',
      '6',
      '7',
      '8',
      '9',
      'a',
      'b',
      'c',
      'd',
      'e',
      'f'};
    put_char(' ');
    // NOLINTNEXTLINE(hicpp-signed-bitwise)
    put_char(hd[(b >> 4U) & 0x0FU]);
    put_char(hd[b & 0x0FU]);
}
//------------------------------------------------------------------------------
template <typename Getter, typename Putter>
void _hexdump_do_hex_dump(span_size_t bgn, Getter get_byte, Putter put_char) {

    bool done = false;
    bool first = true;
    bool prev_same = false;
    span_size_t row = bgn - (bgn % 16);

    std::array<bool, 16> row_none{};
    std::array<byte, 16> row_byte{};
    std::array<byte, 16> row_prev{};

    std::stringstream temp;

    while(not done) {
        span_size_t pos = row;
        bool empty_row = true;
        for(const auto b : integer_range(16)) {
            if(pos < bgn or done) {
                row_none[b] = true;
            } else {
                if(const auto got{get_byte()}) {
                    row_none[b] = false;
                    row_byte[b] = got.value();
                    empty_row = false;
                } else {
                    row_none[b] = true;
                    done = true;
                }
            }
            ++pos;
        }
        if(empty_row) {
            break;
        }

        temp.str({});
        if(first or done or row_prev != row_byte) {
            first = false;
            temp << std::setw(20) << std::setfill('.');
            temp << std::hex << row;
            for(char c : temp.str()) {
                put_char(c);
            }
            put_char('|');

            pos = row;
            for(const auto b : integer_range(16)) {
                if(b == 8) {
                    put_char(' ');
                }

                if(row_none[b]) {
                    put_char(' ');
                    put_char('.');
                    put_char('.');
                } else {
                    _hexdump_to_hex_b(put_char, row_byte[b]);
                }
                ++pos;
            }

            put_char(' ');
            put_char('|');

            pos = row;
            for(const auto b : integer_range(16)) {
                if(b == 8) {
                    put_char(' ');
                }

                if(row_none[b] or not std::isprint(row_byte[b])) {
                    put_char('.');
                } else {
                    put_char(char(row_byte[b]));
                }
                ++pos;
            }
            put_char('|');
            put_char('\n');
            row_prev = row_byte;
            prev_same = false;
        } else if(not prev_same) {
            const auto put_str{[&](const string_view s) {
                for(const char c : s) {
                    put_char(c);
                }
            }};
            const string_view l1{"                   *|"};
            const string_view l2{" .. .. .. .. .. .. .. .. "};
            const string_view l3{"|........ ........|\n"};
            put_str(l1);
            put_str(l2);
            put_str(l2);
            put_str(l3);
            prev_same = true;
        }

        row += 16;
    }
}
//------------------------------------------------------------------------------
// ostream << hexdump
//------------------------------------------------------------------------------
void hexdump::apply(
  const hexdump::byte_getter get_byte,
  const hexdump::char_putter put_char) {

    _hexdump_do_hex_dump(0, get_byte, put_char);
}
//------------------------------------------------------------------------------
auto hexdump::to_stream(std::ostream& out) const noexcept -> std::ostream& {
    out << '\n';

    span_size_t i = 0;

    using memory::as_address;
    _hexdump_do_hex_dump(
      as_address(_mb.begin()).value(),
      make_span_getter(i, _mb),
      [&out](char c) { out << c; });
    return out << std::flush;
}
//------------------------------------------------------------------------------
} // namespace eagine
