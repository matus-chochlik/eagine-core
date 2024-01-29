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

import :interfaces;
import :utilities;

namespace eagine::logs {
//------------------------------------------------------------------------------
// combined sink
//------------------------------------------------------------------------------
class combined_sink final : public stream_sink {
public:
    combined_sink(std::vector<shared_holder<stream_sink>> sinks) noexcept;

    void consume(const begin_info&) noexcept final;
    void consume(const description_info&) noexcept final;
    void consume(const declare_state_info&) noexcept final;
    void consume(const active_state_info&) noexcept final;
    void consume(const message_info&) noexcept final;
    void consume(const interval_info&) noexcept final;
    void consume(const heartbeat_info&) noexcept final;
    void consume(const finish_info&) noexcept final;

private:
    void _consume(const auto& info) noexcept;

    std::vector<shared_holder<stream_sink>> _sinks;
};
//------------------------------------------------------------------------------
combined_sink::combined_sink(
  std::vector<shared_holder<stream_sink>> sinks) noexcept
  : _sinks{std::move(sinks)} {}
//------------------------------------------------------------------------------
void combined_sink::_consume(const auto& info) noexcept {
    for(const auto& sink : _sinks) {
        sink->consume(info);
    }
}
//------------------------------------------------------------------------------
void combined_sink::consume(const begin_info& info) noexcept {
    _consume(info);
}
//------------------------------------------------------------------------------
void combined_sink::consume(const description_info& info) noexcept {
    _consume(info);
}
//------------------------------------------------------------------------------
void combined_sink::consume(const declare_state_info& info) noexcept {
    _consume(info);
}
//------------------------------------------------------------------------------
void combined_sink::consume(const active_state_info& info) noexcept {
    _consume(info);
}
//------------------------------------------------------------------------------
void combined_sink::consume(const message_info& info) noexcept {
    _consume(info);
}
//------------------------------------------------------------------------------
void combined_sink::consume(const interval_info& info) noexcept {
    _consume(info);
}
//------------------------------------------------------------------------------
void combined_sink::consume(const heartbeat_info& info) noexcept {
    _consume(info);
}
//------------------------------------------------------------------------------
void combined_sink::consume(const finish_info& info) noexcept {
    _consume(info);
}
//------------------------------------------------------------------------------
// combined sink factory
//------------------------------------------------------------------------------
class combined_sink_factory final
  : public std::enable_shared_from_this<combined_sink_factory>
  , public stream_sink_factory {
public:
    combined_sink_factory(
      std::vector<shared_holder<stream_sink_factory>> factories);

    auto make_stream() noexcept -> unique_holder<stream_sink> final;
    void update() noexcept final;

private:
    std::vector<shared_holder<stream_sink_factory>> _factories;
};
//------------------------------------------------------------------------------
combined_sink_factory::combined_sink_factory(
  std::vector<shared_holder<stream_sink_factory>> factories)
  : _factories{std::move(factories)} {}
//------------------------------------------------------------------------------
auto combined_sink_factory::make_stream() noexcept
  -> unique_holder<stream_sink> {
    std::vector<shared_holder<stream_sink>> streams;
    for(const auto& factory : _factories) {
        streams.emplace_back(factory->make_stream());
    }

    return {hold<combined_sink>, std::move(streams)};
}
//------------------------------------------------------------------------------
void combined_sink_factory::update() noexcept {
    for(const auto& factory : _factories) {
        factory->update();
    }
}
//------------------------------------------------------------------------------
// factory function
//------------------------------------------------------------------------------
auto make_combined_sink_factory(
  main_ctx&,
  std::vector<shared_holder<stream_sink_factory>> factories) noexcept
  -> shared_holder<stream_sink_factory> {
    return {hold<combined_sink_factory>, std::move(factories)};
}
//------------------------------------------------------------------------------
} // namespace eagine::logs
