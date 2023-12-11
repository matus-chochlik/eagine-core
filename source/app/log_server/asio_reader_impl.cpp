/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/local/stream_protocol.hpp>
#include <asio/signal_set.hpp>

module eagine.core.log_server;

import std;
import eagine.core;

namespace eagine::logs {
//------------------------------------------------------------------------------
// Stream reader
//------------------------------------------------------------------------------
template <typename Acceptor, typename Socket>
class asio_reader_base;

template <typename Acceptor, typename Socket>
class asio_reader_stream
  : public std::enable_shared_from_this<asio_reader_stream<Acceptor, Socket>> {
public:
    asio_reader_stream(
      asio_reader_base<Acceptor, Socket>&,
      Socket socket,
      parser_input sink) noexcept;

    ~asio_reader_stream() noexcept;

    void start();

private:
    void _read();

    asio_reader_base<Acceptor, Socket>& _parent;
    Socket _socket;
    parser_input _sink;
    std::array<char, 1024> _chunk{};
};
//------------------------------------------------------------------------------
// Acceptor base
//------------------------------------------------------------------------------
template <typename Acceptor, typename Socket>
class asio_reader_base
  : public main_ctx_object
  , public reader {
public:
    asio_reader_base(
      identifier id,
      main_ctx& ctx,
      shared_holder<stream_sink_factory> factory,
      auto endpoint) noexcept;

    auto run() noexcept -> bool final;

    void on_read(std::size_t) noexcept;

protected:
    void _accept();
    void _update_later();
    void _notify_later();

    watched_process_lifetime _alive;
    shared_holder<stream_sink_factory> _factory;
    asio::io_context _io{};
    asio::signal_set _signals{_io, SIGINT, SIGTERM};
    asio::steady_timer _update_timer{_io};
    asio::steady_timer _alive_timer{_io};
    Acceptor _acceptor;
    Socket _socket{_io};
    resetting_timeout _should_heartbeat{std::chrono::minutes{1}};
    std::size_t _batch_count{0U};
    std::size_t _batch_read{0U};
};
//------------------------------------------------------------------------------
template <typename Acceptor, typename Socket>
void asio_reader_stream<Acceptor, Socket>::start() {
    _read();
}
//------------------------------------------------------------------------------
template <typename Acceptor, typename Socket>
void asio_reader_stream<Acceptor, Socket>::_read() {
    auto self{this->shared_from_this()};
    _socket.async_read_some(
      asio::buffer(_chunk.data(), _chunk.size()),
      [this, self](std::error_code error, std::size_t size) {
          if(not error) {
              _sink.consume_text(head(view(_chunk), span_size(size)));
              _parent.on_read(size);
              _read();
          }
      });
}
//------------------------------------------------------------------------------
template <typename Acceptor, typename Socket>
asio_reader_stream<Acceptor, Socket>::asio_reader_stream(
  asio_reader_base<Acceptor, Socket>& parent,
  Socket socket,
  parser_input sink) noexcept
  : _parent{parent}
  , _socket{std::move(socket)}
  , _sink{std::move(sink)} {}
//------------------------------------------------------------------------------
template <typename Acceptor, typename Socket>
asio_reader_stream<Acceptor, Socket>::~asio_reader_stream() noexcept {
    _sink.finish();
}
//------------------------------------------------------------------------------
template <typename Acceptor, typename Socket>
void asio_reader_base<Acceptor, Socket>::_accept() {
    _acceptor.async_accept(_socket, [this](std::error_code error) {
        if(not error) {
            log_info("starting new log stream");
            std::make_shared<asio_reader_stream<Acceptor, Socket>>(
              *this,
              std::move(_socket),
              make_data_parser(main_context(), _factory->make_stream()))
              ->start();
            _factory->update();
        }
        _accept();
    });
}
//------------------------------------------------------------------------------
template <typename Acceptor, typename Socket>
void asio_reader_base<Acceptor, Socket>::_update_later() {
    _update_timer.expires_after(std::chrono::seconds{5});
    _update_timer.async_wait([this](std::error_code error) {
        if(not error) {
            _factory->update();
        }
        _notify_later();
    });
}
//------------------------------------------------------------------------------
template <typename Acceptor, typename Socket>
void asio_reader_base<Acceptor, Socket>::_notify_later() {
    _alive_timer.expires_after(std::chrono::seconds{10});
    _alive_timer.async_wait([this](std::error_code error) {
        if(not error) {
            _alive.notify();
        }
        _notify_later();
    });
    if(_should_heartbeat) {
        this->heartbeat();
    }
}
//------------------------------------------------------------------------------
template <typename Acceptor, typename Socket>
asio_reader_base<Acceptor, Socket>::asio_reader_base(
  identifier id,
  main_ctx& ctx,
  shared_holder<stream_sink_factory> factory,
  auto endpoint) noexcept
  : main_ctx_object{id, ctx}
  , _alive{ctx.watchdog().start_watch()}
  , _factory{std::move(factory)}
  , _acceptor{_io, std::move(endpoint)} {

    _signals.async_wait([&](auto, auto) { _io.stop(); });
    _accept();
    _update_later();
    _notify_later();
}
//------------------------------------------------------------------------------
template <typename Acceptor, typename Socket>
void asio_reader_base<Acceptor, Socket>::on_read(std::size_t size) noexcept {
    const std::size_t batch{1024U * 1024U * 1024U};

    _batch_read += size;
    if(_batch_read > batch) {
        _batch_read -= batch;
        ++_batch_count;
        log_stat("read ${count} * ${batch} of log data")
          .tag("readBytes")
          .arg("count", _batch_count)
          .arg("batch", "ByteSize", batch);
    }
    _factory->update();
}
//------------------------------------------------------------------------------
template <typename Acceptor, typename Socket>
auto asio_reader_base<Acceptor, Socket>::run() noexcept -> bool {
    try {
        while(not _io.stopped()) {
            _io.run_one();
            std::this_thread::sleep_for(std::chrono::milliseconds{1});
        }
        return true;
    } catch(std::exception& error) {
        log_error("ASIO reader error: ${what}").arg("what", error.what());
        return false;
    }
}
//------------------------------------------------------------------------------
// Unix local reader
//------------------------------------------------------------------------------
class asio_local_reader final
  : public asio_reader_base<
      asio::local::stream_protocol::acceptor,
      asio::local::stream_protocol::socket> {
public:
    asio_local_reader(
      main_ctx& ctx,
      shared_holder<stream_sink_factory> factory,
      const string_view address) noexcept;

    ~asio_local_reader() noexcept;

private:
    auto _get_path(const string_view) noexcept;
    auto _get_ept(const string_view) noexcept;

    std::string _path;
};
//------------------------------------------------------------------------------
auto asio_local_reader::_get_path(const string_view path) noexcept {
    return path ? path : string_view{"/tmp/eagine-log"};
}
//------------------------------------------------------------------------------
auto asio_local_reader::_get_ept(const string_view address) noexcept {
    return asio::local::stream_protocol::endpoint{_get_path(address)};
}
//------------------------------------------------------------------------------
asio_local_reader::asio_local_reader(
  main_ctx& ctx,
  shared_holder<stream_sink_factory> factory,
  string_view address) noexcept
  : asio_reader_base{"ALclReader", ctx, std::move(factory), _get_ept(address)}
  , _path{_get_path(address)} {}
//------------------------------------------------------------------------------
asio_local_reader::~asio_local_reader() noexcept {
    try {
        std::remove(_path.c_str());
    } catch(...) {
    }
}
//------------------------------------------------------------------------------
// TCP/IPv4 reader
//------------------------------------------------------------------------------
class asio_tcp_ipv4_reader final
  : public asio_reader_base<asio::ip::tcp::acceptor, asio::ip::tcp::socket> {
public:
    asio_tcp_ipv4_reader(
      main_ctx& ctx,
      shared_holder<stream_sink_factory> factory,
      const string_view address) noexcept;

private:
    auto _get_addr(const string_view) noexcept;
    auto _get_port(const string_view) noexcept -> unsigned short;
};
//------------------------------------------------------------------------------
auto asio_tcp_ipv4_reader::_get_addr(const string_view) noexcept {
    return asio::ip::tcp::v4();
}
//------------------------------------------------------------------------------
auto asio_tcp_ipv4_reader::_get_port(const string_view) noexcept
  -> unsigned short {
    return 34917;
}
//------------------------------------------------------------------------------
asio_tcp_ipv4_reader::asio_tcp_ipv4_reader(
  main_ctx& ctx,
  shared_holder<stream_sink_factory> factory,
  string_view address) noexcept
  : asio_reader_base{
      "ANw4Reader",
      ctx,
      std::move(factory),
      asio::ip::tcp::endpoint{_get_addr(address), _get_port(address)}} {}
//------------------------------------------------------------------------------
// make readers
//------------------------------------------------------------------------------
auto make_asio_local_reader(
  main_ctx& ctx,
  shared_holder<stream_sink_factory> factory,
  string_view address) noexcept -> unique_holder<reader> {
    return {hold<asio_local_reader>, ctx, std::move(factory), address};
}
//------------------------------------------------------------------------------
auto make_asio_tcp_ipv4_reader(
  main_ctx& ctx,
  shared_holder<stream_sink_factory> factory,
  string_view address) noexcept -> unique_holder<reader> {
    return {hold<asio_tcp_ipv4_reader>, ctx, std::move(factory), address};
}
//------------------------------------------------------------------------------
} // namespace eagine::logs
