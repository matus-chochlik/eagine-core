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

import :interfaces;

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
    void _on_read(std::size_t size) noexcept;

    main_ctx& _ctx;
    std::istream& _input;
    shared_holder<stream_sink_factory> _factory;
    parser_input _sink;
    std::size_t _batch_count{0U};
    std::size_t _batch_read{0U};
};
//------------------------------------------------------------------------------
istream_reader::istream_reader(
  main_ctx& ctx,
  std::istream& input,
  shared_holder<stream_sink_factory> factory) noexcept
  : _ctx{ctx}
  , _input{input}
  , _factory{std::move(factory)}
  , _sink{make_data_parser(ctx, _factory->make_stream())} {
    _factory->update();
}
//------------------------------------------------------------------------------
void istream_reader::_on_read(std::size_t size) noexcept {
    const std::size_t batch{1024U * 1024U * 1024U};

    _batch_read += size;
    if(_batch_read > batch) {
        _batch_read -= batch;
        ++_batch_count;
        _ctx.log()
          .stat("read ${count} * ${batch} of log data")
          .tag("readBytes")
          .arg("count", _batch_count)
          .arg("batch", "ByteSize", batch);
    }
}
//------------------------------------------------------------------------------
auto istream_reader::run() noexcept -> bool {
    if(_input.good()) {
        std::array<char, 1024> chunk{};
        resetting_timeout should_heartbeat{std::chrono::minutes{1}};
        if(auto alive{_ctx.watchdog().start_watch()}) {
            while(not _input.eof()) {
                _input.read(
                  static_cast<char*>(chunk.data()),
                  static_cast<std::streamsize>(chunk.size()));
                if(_input.bad()) {
                    return false;
                }
                const auto size{_input.gcount()};
                _sink.consume_text(head(view(chunk), span_size(size)));
                _on_read(std_size(size));
                _factory->update();
                alive.notify();

                if(should_heartbeat) {
                    _ctx.log().heartbeat();
                }
            }
        }
        _sink.finish();
        _factory->update();
        return true;
    }
    return false;
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
