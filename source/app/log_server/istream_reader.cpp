/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include "interfaces.hpp"

namespace eagine::logs {
//------------------------------------------------------------------------------
// reader
//------------------------------------------------------------------------------
class istream_reader final : public reader {
public:
    istream_reader(
      main_ctx& ctx,
      std::istream& input,
      shared_holder<stream_sink_factory> factory) noexcept;

    auto run() noexcept -> bool final;

private:
    std::istream& _input;
    valtree::value_tree_stream_input _sink;
};
//------------------------------------------------------------------------------
istream_reader::istream_reader(
  main_ctx& ctx,
  std::istream& input,
  shared_holder<stream_sink_factory> factory) noexcept
  : _input{input}
  , _sink{make_data_parser(ctx, factory->make_stream())} {}
//------------------------------------------------------------------------------
auto istream_reader::run() noexcept -> bool {
    std::array<char, 4096> chunk{};
    while(not _input.eof()) {
        _input.read(
          static_cast<char*>(chunk.data()),
          static_cast<std::streamsize>(chunk.size()));
        if(_input.bad()) {
            return false;
        }
        _sink.consume_text(head(view(chunk), span_size(_input.gcount())));
    }
    _sink.finish();
    return true;
}
//------------------------------------------------------------------------------
// make reader
//------------------------------------------------------------------------------
auto make_cin_reader(
  main_ctx& ctx,
  shared_holder<stream_sink_factory> factory) noexcept
  -> unique_holder<reader> {
    return {hold<istream_reader>, ctx, std::cin, std::move(factory)};
}
//------------------------------------------------------------------------------
} // namespace eagine::logs
