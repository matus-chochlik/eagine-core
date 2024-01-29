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
// combined output
//------------------------------------------------------------------------------
class combined_output final : public text_output {
public:
    combined_output(std::vector<unique_holder<text_output>>) noexcept;

protected:
    void write(string_view) noexcept final;
    void flush() noexcept final;

private:
    std::vector<unique_holder<text_output>> _outputs;
};
//------------------------------------------------------------------------------
combined_output::combined_output(
  std::vector<unique_holder<text_output>> outputs) noexcept
  : _outputs{std::move(outputs)} {}
//------------------------------------------------------------------------------
void combined_output::write(string_view s) noexcept {
    for(auto& output : _outputs) {
        output->write(s);
    }
}
//------------------------------------------------------------------------------
void combined_output::flush() noexcept {
    for(auto& output : _outputs) {
        output->flush();
    }
}
//------------------------------------------------------------------------------
// make factory
//------------------------------------------------------------------------------
auto make_combined_text_output(std::vector<unique_holder<text_output>> outputs)
  -> unique_holder<text_output> {
    return {hold<combined_output>, std::move(outputs)};
}
//------------------------------------------------------------------------------
} // namespace eagine::logs
