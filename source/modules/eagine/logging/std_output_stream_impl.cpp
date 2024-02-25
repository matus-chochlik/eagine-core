/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module eagine.core.logging;

import std;
import eagine.core.types;
import eagine.core.memory;
import :backend;

namespace eagine {
//------------------------------------------------------------------------------
class std_log_output_stream : public log_output_stream {
public:
    std_log_output_stream(std::ostream& out) noexcept
      : _out{out} {}

    void begin_stream(
      const memory::const_block&,
      const memory::const_block&) noexcept final;
    void begin_entry() noexcept final;
    void entry_append(const memory::const_block&) noexcept final;
    void finish_entry() noexcept final;
    void finish_stream(const memory::const_block&) noexcept final;

private:
    void _write(const memory::const_block&) noexcept;
    void _flush() noexcept;

    memory::buffer _sep;
    std::ostream& _out;
    bool _first{true};
};
//------------------------------------------------------------------------------
void std_log_output_stream::_write(const memory::const_block& chunk) noexcept {
    try {
        memory::write_to_stream(_out, chunk);
    } catch(...) {
    }
}
//------------------------------------------------------------------------------
void std_log_output_stream::_flush() noexcept {
    try {
        _out.flush();
    } catch(...) {
    }
}
//------------------------------------------------------------------------------
void std_log_output_stream::begin_stream(
  const memory::const_block& stream_header,
  const memory::const_block& entry_sep) noexcept {
    _write(stream_header);
    memory::append_to(entry_sep, _sep);
}
//------------------------------------------------------------------------------
void std_log_output_stream::begin_entry() noexcept {
    if(not _first) [[likely]] {
        _write(view(_sep));
    } else {
        _first = false;
    }
}
//------------------------------------------------------------------------------
void std_log_output_stream::entry_append(const memory::const_block& c) noexcept {
    _write(c);
}
//------------------------------------------------------------------------------
void std_log_output_stream::finish_entry() noexcept {
    _flush();
}
//------------------------------------------------------------------------------
void std_log_output_stream::finish_stream(
  const memory::const_block& stream_footer) noexcept {
    _write(stream_footer);
    _flush();
}
//------------------------------------------------------------------------------
auto make_std_output_stream(std::ostream& out)
  -> unique_holder<log_output_stream> {
    return {hold<std_log_output_stream>, out};
}
//------------------------------------------------------------------------------
} // namespace eagine
