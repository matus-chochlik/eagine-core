/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module eagine.core.log_server;

import std;
import eagine.core;

namespace eagine::logs {
//------------------------------------------------------------------------------
// ostream output
//------------------------------------------------------------------------------
class ostream_output final : public formatted_output {
public:
    ostream_output(std::ostream&) noexcept;
    ~ostream_output() noexcept;

protected:
    void write(string_view) noexcept final;
    void update() noexcept final {}

private:
    std::ostream& _output;
};
//------------------------------------------------------------------------------
ostream_output::ostream_output(std::ostream& output) noexcept
  : _output{output} {
    _start();
}
//------------------------------------------------------------------------------
ostream_output::~ostream_output() noexcept {
    _finish();
}
//------------------------------------------------------------------------------
void ostream_output::write(string_view s) noexcept {
    write_to_stream(_output, s);
}
//------------------------------------------------------------------------------
// make factory
//------------------------------------------------------------------------------
auto make_ostream_sink_factory(main_ctx&) noexcept
  -> shared_holder<stream_sink_factory> {
    return {hold<ostream_output>, std::cout};
}
//------------------------------------------------------------------------------
} // namespace eagine::logs
