/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module;

#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/local/stream_protocol.hpp>
#include <asio/write.hpp>

module eagine.core.logging;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.string;
import eagine.core.utility;
import :backend;

namespace eagine {
//------------------------------------------------------------------------------
// Stream base
//------------------------------------------------------------------------------
template <typename Endpoint, typename Socket>
class asio_log_output_stream_base
  : public std::enable_shared_from_this<
      asio_log_output_stream_base<Endpoint, Socket>>
  , public log_output_stream {
public:
    asio_log_output_stream_base(Endpoint endpoint);
    void write(const memory::const_block&) noexcept final;
    void flush(bool) noexcept final;

private:
    void _write() noexcept;

    asio::io_context _context{};
    Endpoint _endpoint;
    Socket _socket;
    double_buffer<memory::buffer> _buffers;
};
//------------------------------------------------------------------------------
template <typename Endpoint, typename Socket>
asio_log_output_stream_base<Endpoint, Socket>::asio_log_output_stream_base(
  Endpoint endpoint)
  : _endpoint{std::move(endpoint)}
  , _socket{_context} {}
//------------------------------------------------------------------------------
template <typename Endpoint, typename Socket>
void asio_log_output_stream_base<Endpoint, Socket>::_write() noexcept {
    _buffers.swap();
    _buffers.next().clear();
    if(auto& chunk{_buffers.current()}; not chunk.empty()) {
        asio::write(
          _socket, asio::buffer(chunk.data(), std_size(chunk.size())));
    }
}
//------------------------------------------------------------------------------
template <typename Endpoint, typename Socket>
void asio_log_output_stream_base<Endpoint, Socket>::write(
  const memory::const_block& chunk) noexcept {
    append_to(chunk, _buffers.next());
}
//------------------------------------------------------------------------------
template <typename Endpoint, typename Socket>
void asio_log_output_stream_base<Endpoint, Socket>::flush(bool) noexcept {
    if(_socket.is_open()) [[likely]] {
        _write();
    } else {
        _socket.connect(_endpoint);
        _socket.set_option(typename Socket::keep_alive{true});
        if(_socket.is_open()) {
            _write();
        }
    }
}
//------------------------------------------------------------------------------
// Local stream
//------------------------------------------------------------------------------
class asio_local_log_output_stream
  : public asio_log_output_stream_base<
      asio::local::stream_protocol::endpoint,
      asio::local::stream_protocol::socket> {
    using base = asio_log_output_stream_base<
      asio::local::stream_protocol::endpoint,
      asio::local::stream_protocol::socket>;

public:
    asio_local_log_output_stream(const string_view addr_str)
      : base{_endpoint(addr_str)} {}

private:
    static auto _fix_addr(const string_view addr_str) noexcept -> string_view {
        return addr_str ? addr_str : string_view{"/tmp/eagine-log"};
    }

    static auto _endpoint(const string_view addr_str) noexcept
      -> asio::local::stream_protocol::endpoint {
        return {_fix_addr(addr_str)};
    }
};
//------------------------------------------------------------------------------
// TCP/IPv4 stream
//------------------------------------------------------------------------------
class asio_tcpipv4_log_output_stream
  : public asio_log_output_stream_base<
      asio::ip::tcp::endpoint,
      asio::ip::tcp::socket> {
    using base =
      asio_log_output_stream_base<asio::ip::tcp::endpoint, asio::ip::tcp::socket>;

public:
    asio_tcpipv4_log_output_stream(const string_view addr_str)
      : base{_endpoint(addr_str)} {}

private:
    static auto _fix_addr(const string_view addr_str)
      -> std::tuple<std::string, std::string> {
        auto [hostname, port_str] = split_by_last(addr_str, string_view(":"));
        return {
          hostname ? to_string(hostname) : std::string("localhost"),
          port_str ? to_string(port_str) : std::string("34917")};
    }

    auto _endpoint(const string_view addr_str) noexcept
      -> asio::ip::tcp::endpoint {
        const auto [host, port]{_fix_addr(addr_str)};
        asio::io_context context{};
        asio::ip::tcp::resolver resolver{context};
        for(const auto& endpt : resolver.resolve(host, port)) {
            return endpt;
        }
        return {};
    }
};
//------------------------------------------------------------------------------
// factory functions
//------------------------------------------------------------------------------
auto make_asio_local_output_stream(const string_view addr_str)
  -> unique_holder<log_output_stream> {
    return {hold<asio_local_log_output_stream>, addr_str};
}
//------------------------------------------------------------------------------
auto make_asio_tcpipv4_output_stream(const string_view addr_str)
  -> unique_holder<log_output_stream> {
    return {hold<asio_tcpipv4_log_output_stream>, addr_str};
}
//------------------------------------------------------------------------------
} // namespace eagine
  //
