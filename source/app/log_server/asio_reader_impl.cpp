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
template <typename Socket>
class asio_reader_stream
  : public std::enable_shared_from_this<asio_reader_stream<Socket>> {
public:
    asio_reader_stream(Socket socket, parser_input sink) noexcept;

    void start();

private:
    void _read();

    Socket _socket;
    parser_input _sink;
    std::array<char, 1024> _chunk{};
};
//------------------------------------------------------------------------------
template <typename Socket>
void asio_reader_stream<Socket>::start() {
    _read();
}
//------------------------------------------------------------------------------
template <typename Socket>
void asio_reader_stream<Socket>::_read() {
    auto self{this->shared_from_this()};
    _socket.async_read_some(
      asio::buffer(_chunk.data(), _chunk.size()),
      [this, self](std::error_code error, std::size_t size) {
          if(not error) {
              _sink.consume_text(head(view(_chunk), span_size(size)));
              _read();
          }
      });
}
//------------------------------------------------------------------------------
template <typename Socket>
asio_reader_stream<Socket>::asio_reader_stream(
  Socket socket,
  parser_input sink) noexcept
  : _socket{std::move(socket)}
  , _sink{std::move(sink)} {}
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

protected:
    void _accept();

    shared_holder<stream_sink_factory> _factory;
    asio::io_context _io{1};
    asio::signal_set _signals{_io, SIGINT, SIGTERM};
    Acceptor _acceptor;
    Socket _socket{_io};
};
//------------------------------------------------------------------------------
template <typename Acceptor, typename Socket>
void asio_reader_base<Acceptor, Socket>::_accept() {
    _acceptor.async_accept(_socket, [this](std::error_code error) {
        if(not error) {
            std::make_shared<asio_reader_stream<Socket>>(
              std::move(_socket),
              make_data_parser(main_context(), _factory->make_stream()))
              ->start();
        }
        _accept();
    });
}
//------------------------------------------------------------------------------
template <typename Acceptor, typename Socket>
asio_reader_base<Acceptor, Socket>::asio_reader_base(
  identifier id,
  main_ctx& ctx,
  shared_holder<stream_sink_factory> factory,
  auto endpoint) noexcept
  : main_ctx_object{id, ctx}
  , _factory{std::move(factory)}
  , _acceptor{_io, std::move(endpoint)} {

    _signals.async_wait([&](auto, auto) { _io.stop(); });
    _accept();
}
//------------------------------------------------------------------------------
template <typename Acceptor, typename Socket>
auto asio_reader_base<Acceptor, Socket>::run() noexcept -> bool {
    try {
        _io.run();
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

private:
    auto _get_path(const string_view) noexcept;
};
//------------------------------------------------------------------------------
auto asio_local_reader::_get_path(const string_view path) noexcept {
    return path ? path : string_view{"/tmp/eagine-xmllog"};
}
//------------------------------------------------------------------------------
asio_local_reader::asio_local_reader(
  main_ctx& ctx,
  shared_holder<stream_sink_factory> factory,
  string_view address) noexcept
  : asio_reader_base{
      "ANw4Reader",
      ctx,
      std::move(factory),
      asio::local::stream_protocol::endpoint{_get_path(address)}} {}
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

