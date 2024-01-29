/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module eagine.core.log_server;

import std;
import eagine.core;

namespace eagine::logs {
//------------------------------------------------------------------------------
// ostream output
//------------------------------------------------------------------------------
class ostream_output final : public text_output {
public:
    ostream_output(std::ostream&) noexcept;

protected:
    void write(string_view) noexcept final;
    void flush() noexcept final;

private:
    std::ostream& _output;
};
//------------------------------------------------------------------------------
ostream_output::ostream_output(std::ostream& output) noexcept
  : _output{output} {}
//------------------------------------------------------------------------------
void ostream_output::write(string_view s) noexcept {
    write_to_stream(_output, s);
}
//------------------------------------------------------------------------------
void ostream_output::flush() noexcept {
    _output.flush();
}
//------------------------------------------------------------------------------
// make factory
//------------------------------------------------------------------------------
auto make_ostream_text_output(main_ctx&) -> unique_holder<text_output> {
    return {hold<ostream_output>, std::cout};
}
//------------------------------------------------------------------------------
} // namespace eagine::logs
