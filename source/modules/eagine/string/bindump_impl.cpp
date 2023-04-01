/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module eagine.core.string;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.utility;

namespace eagine {
//------------------------------------------------------------------------------
template <typename Putter>
void _bindump_to_bin_b(Putter& put_char, byte b) {
    static const char bd[2] = {'0', '1'};
    for(const auto o : integer_range(4U)) {
        byte c = (b >> ((4 - o - 1) * 2));
        put_char(' ');
        put_char(bd[(c >> 1U) & 0x01U]); // NOLINT(hicpp-signed-bitwise)
        put_char(bd[c & 0x01U]);
    }
}
//------------------------------------------------------------------------------
template <typename Getter, typename Putter>
void _bindump_do_bin_dump(
  const span_size_t bgn,
  Getter get_byte,
  Putter put_char) {

    bool done = false;
    span_size_t row = bgn - (bgn % 4);

    bool row_none[4]{};
    byte row_byte[4]{};

    while(not done) {
        span_size_t pos = row;
        bool empty_row = true;
        for(const auto b : integer_range(4)) {
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

        std::stringstream temp;
        temp << std::setw(20) << std::setfill('.');
        temp << std::hex << row;
        for(char c : temp.str()) {
            put_char(c);
        }
        put_char('|');

        pos = row;
        for(const auto b : integer_range(4)) {
            if(b == 8) {
                put_char(' ');
            }

            if(row_none[b]) {
                put_char(' ');
                put_char('.');
                put_char('.');
            } else {
                put_char(' ');
                _bindump_to_bin_b(put_char, row_byte[b]);
            }
            ++pos;
        }

        put_char(' ');
        put_char('|');

        pos = row;
        for(const auto b : integer_range(4)) {
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

        row += 4;
        put_char('|');
        put_char('\n');
    }
}
//------------------------------------------------------------------------------
// ostream << bindump
//------------------------------------------------------------------------------
void bindump::apply(
  const bindump::byte_getter get_byte,
  const bindump::char_putter put_char) {

    _bindump_do_bin_dump(0, get_byte, put_char);
}
//------------------------------------------------------------------------------
auto bindump::to_stream(std::ostream& out) const noexcept -> std::ostream& {
    out << '\n';

    span_size_t i = 0;

    using memory::as_address;
    _bindump_do_bin_dump(
      as_address(_mb.begin()).value(),
      make_span_getter(i, _mb),
      [&out](char c) { out << c; });
    return out << std::flush;
}
//------------------------------------------------------------------------------
} // namespace eagine
