/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
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

    void write(const memory::const_block&) noexcept final;
    void flush(bool) noexcept final;

private:
    std::ostream& _out;
};
//------------------------------------------------------------------------------
void std_log_output_stream::write(const memory::const_block& chunk) noexcept {
    try {
        memory::write_to_stream(_out, chunk);
    } catch(...) {
    }
}
//------------------------------------------------------------------------------
void std_log_output_stream::flush(bool force) noexcept {
    try {
        if(force) {
            _out.flush();
        }
    } catch(...) {
    }
}
//------------------------------------------------------------------------------
auto make_std_output_stream(std::ostream& out)
  -> unique_holder<log_output_stream> {
    return {hold<std_log_output_stream>, out};
}
//------------------------------------------------------------------------------
} // namespace eagine
