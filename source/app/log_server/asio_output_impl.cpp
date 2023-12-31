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
#include <asio/write.hpp>

module eagine.core.log_server;

import std;
import eagine.core;

namespace eagine::logs {
//------------------------------------------------------------------------------
// stream sink
//------------------------------------------------------------------------------
template <typename Socket>
class asio_output_stream
  : public std::enable_shared_from_this<asio_output_stream<Socket>> {
public:
    asio_output_stream(
      Socket socket,
      shared_holder<memory::buffer_pool>) noexcept;
    ~asio_output_stream() noexcept;

    auto is_connected() noexcept -> bool;
    void write(const memory::const_block) noexcept;

private:
    void _write();
    shared_holder<memory::buffer_pool> _pool;
    double_buffer<memory::buffer> _buffers;
    Socket _socket;
    bool _failed{false};
    bool _writing{false};
};
//------------------------------------------------------------------------------
template <typename Socket>
asio_output_stream<Socket>::asio_output_stream(
  Socket socket,
  shared_holder<memory::buffer_pool> pool) noexcept
  : _pool{std::move(pool)}
  , _buffers{_pool->get(), _pool->get()}
  , _socket{std::move(socket)} {}
//------------------------------------------------------------------------------
template <typename Socket>
asio_output_stream<Socket>::~asio_output_stream() noexcept {
    _pool->eat(std::move(_buffers.back()));
    _pool->eat(std::move(_buffers.front()));
}
//------------------------------------------------------------------------------
template <typename Socket>
auto asio_output_stream<Socket>::is_connected() noexcept -> bool {
    return not _failed;
}
//------------------------------------------------------------------------------
template <typename Socket>
void asio_output_stream<Socket>::write(const memory::const_block data) noexcept {
    append_to(data, _buffers.next());
    if(not _writing) {
        _write();
    }
}
//------------------------------------------------------------------------------
template <typename Socket>
void asio_output_stream<Socket>::_write() {
    _buffers.swap();
    _buffers.next().clear();
    if(auto& chunk{_buffers.current()}; not chunk.empty()) {
        _writing = true;
        asio::async_write(
          _socket,
          asio::buffer(chunk.data(), std_size(chunk.size())),
          [this, self{this->shared_from_this()}](
            std::error_code error, std::size_t size) {
              if(not error) {
                  _write();
              } else {
                  _failed = true;
              }
          });
    } else {
        _writing = false;
    }
}
//------------------------------------------------------------------------------
// Acceptor base
//------------------------------------------------------------------------------
template <typename Acceptor, typename Socket>
class asio_output_base : public text_output {
public:
    asio_output_base(main_ctx& ctx, auto endpoint) noexcept;

    void write(const string_view) noexcept final;
    void flush() noexcept final;

protected:
    void _accept();

    asio::io_context _io{};
    Acceptor _acceptor;
    Socket _socket{_io};

    shared_holder<memory::buffer_pool> _pool;
    small_vector<shared_holder<asio_output_stream<Socket>>, 4> _clients;
};
//------------------------------------------------------------------------------
template <typename Acceptor, typename Socket>
void asio_output_base<Acceptor, Socket>::_accept() {
    _acceptor.async_accept(_socket, [this](std::error_code error) {
        if(not error) {
            _clients.emplace_back(std::make_shared<asio_output_stream<Socket>>(
              std::move(_socket), _pool));
        }
        _accept();
    });
}
//------------------------------------------------------------------------------
template <typename Acceptor, typename Socket>
asio_output_base<Acceptor, Socket>::asio_output_base(
  main_ctx& ctx,
  auto endpoint) noexcept
  : _acceptor{_io, std::move(endpoint)} {
    _pool.ensure();
    _accept();
}
//------------------------------------------------------------------------------
template <typename Acceptor, typename Socket>
void asio_output_base<Acceptor, Socket>::write(const string_view text) noexcept {
    _clients.erase_if([](auto& client) { return not client->is_connected(); });
    for(auto& client : _clients) {
        client->write(as_bytes(text));
    }
}
//------------------------------------------------------------------------------
template <typename Acceptor, typename Socket>
void asio_output_base<Acceptor, Socket>::flush() noexcept {
    _io.poll();
}
//------------------------------------------------------------------------------
// Unix Local output
//------------------------------------------------------------------------------
class asio_local_output final
  : public asio_output_base<
      asio::local::stream_protocol::acceptor,
      asio::local::stream_protocol::socket> {
public:
    asio_local_output(main_ctx& ctx, const string_view address) noexcept;
    ~asio_local_output() noexcept;

private:
    auto _get_path(const string_view) noexcept;
    auto _get_ept(const string_view) noexcept;

    std::string _path;
};
//------------------------------------------------------------------------------
auto asio_local_output::_get_path(const string_view path) noexcept {
    return path ? path : string_view{"/tmp/eagine-text-log"};
}
//------------------------------------------------------------------------------
auto asio_local_output::_get_ept(const string_view address) noexcept {
    return asio::local::stream_protocol::endpoint{_get_path(address)};
}
//------------------------------------------------------------------------------
asio_local_output::asio_local_output(main_ctx& ctx, string_view path) noexcept
  : asio_output_base{ctx, _get_ept(_get_path(path))}
  , _path{_get_path(path)} {}
//------------------------------------------------------------------------------
asio_local_output::~asio_local_output() noexcept {
    try {
        std::remove(_path.c_str());
    } catch(...) {
    }
}
//------------------------------------------------------------------------------
// TCP/IPv4 output
//------------------------------------------------------------------------------
class asio_tcp_ipv4_output final
  : public asio_output_base<asio::ip::tcp::acceptor, asio::ip::tcp::socket> {
public:
    asio_tcp_ipv4_output(main_ctx& ctx, const string_view address) noexcept;

private:
    auto _get_addr(const string_view) noexcept;
    auto _get_port(const string_view) noexcept -> unsigned short;
};
//------------------------------------------------------------------------------
auto asio_tcp_ipv4_output::_get_addr(const string_view) noexcept {
    return asio::ip::tcp::v4();
}
//------------------------------------------------------------------------------
auto asio_tcp_ipv4_output::_get_port(const string_view) noexcept
  -> unsigned short {
    return 34915;
}
//------------------------------------------------------------------------------
asio_tcp_ipv4_output::asio_tcp_ipv4_output(
  main_ctx& ctx,
  string_view address) noexcept
  : asio_output_base{
      ctx,
      asio::ip::tcp::endpoint{_get_addr(address), _get_port(address)}} {}
//------------------------------------------------------------------------------
// factory functions
//------------------------------------------------------------------------------
auto make_asio_local_text_output(main_ctx& ctx, string_view address)
  -> unique_holder<text_output> {
    return {hold<asio_local_output>, ctx, address};
}
//------------------------------------------------------------------------------
auto make_asio_tcp_ipv4_text_output(main_ctx& ctx, string_view address)
  -> unique_holder<text_output> {
    return {hold<asio_tcp_ipv4_output>, ctx, address};
}
//------------------------------------------------------------------------------
} // namespace eagine::logs
