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
import eagine.core.runtime;
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
    void begin_stream(
      const memory::const_block&,
      const memory::const_block&) noexcept final;
    void begin_entry() noexcept final;
    void entry_append(const memory::const_block&) noexcept final;
    void finish_entry() noexcept final;
    void finish_stream(const memory::const_block&) noexcept final;

private:
    void _send(const memory::const_block) noexcept;
    void _store(const memory::const_block) noexcept;
    void _send_stored() noexcept;
    void _reconnect() noexcept;
    void _handle_entry() noexcept;

    asio::io_context _context{};
    Endpoint _endpoint;
    Socket _socket;
    timeout _should_reconnect{std::chrono::seconds{1}, nothing};
    double_buffer<memory::buffer> _buffers;
    temporary_chunk_storage _storage;
    memory::buffer _bgn;
    memory::buffer _sep;
    bool _first{true};
};
//------------------------------------------------------------------------------
template <typename Endpoint, typename Socket>
asio_log_output_stream_base<Endpoint, Socket>::asio_log_output_stream_base(
  Endpoint endpoint)
  : _endpoint{std::move(endpoint)}
  , _socket{_context} {}
//------------------------------------------------------------------------------
template <typename Endpoint, typename Socket>
void asio_log_output_stream_base<Endpoint, Socket>::_send_stored() noexcept {
    using Self = asio_log_output_stream_base;
    _storage.for_each_chunk({this, member_function_constant_t<&Self::_send>{}})
      .clear();
}
//------------------------------------------------------------------------------
template <typename Endpoint, typename Socket>
void asio_log_output_stream_base<Endpoint, Socket>::_send(
  const memory::const_block chunk) noexcept {
    asio::write(_socket, asio::buffer(chunk.data(), std_size(chunk.size())));
}
//------------------------------------------------------------------------------
template <typename Endpoint, typename Socket>
void asio_log_output_stream_base<Endpoint, Socket>::_store(
  const memory::const_block chunk) noexcept {
    _storage.add_chunk(chunk);
}
//------------------------------------------------------------------------------
template <typename Endpoint, typename Socket>
void asio_log_output_stream_base<Endpoint, Socket>::_handle_entry() noexcept {
    _buffers.swap();
    _buffers.next().clear();
    if(auto& chunk{_buffers.current()}; not chunk.empty()) {
        if(_socket.is_open()) [[likely]] {
            try {
                if(not _storage.empty()) [[unlikely]] {
                    _send_stored();
                }
                _send(view(chunk));
            } catch(...) {
                _socket.close();
                _store(view(chunk));
            }
        } else {
            _store(view(chunk));
        }
    }
}
//------------------------------------------------------------------------------
template <typename Endpoint, typename Socket>
void asio_log_output_stream_base<Endpoint, Socket>::begin_stream(
  const memory::const_block& bgn,
  const memory::const_block& sep) noexcept {
    memory::append_to(bgn, _bgn);
    memory::append_to(sep, _sep);
}
//------------------------------------------------------------------------------
template <typename Endpoint, typename Socket>
void asio_log_output_stream_base<Endpoint, Socket>::begin_entry() noexcept {
    if(not _first) [[likely]] {
        append_to(view(_sep), _buffers.next());
    } else {
        append_to(view(_bgn), _buffers.next());
        _first = false;
    }
}
//------------------------------------------------------------------------------
template <typename Endpoint, typename Socket>
void asio_log_output_stream_base<Endpoint, Socket>::entry_append(
  const memory::const_block& chunk) noexcept {
    append_to(chunk, _buffers.next());
}
//------------------------------------------------------------------------------
template <typename Endpoint, typename Socket>
void asio_log_output_stream_base<Endpoint, Socket>::_reconnect() noexcept {
    if(_should_reconnect) {
        try {
            _socket.connect(_endpoint);
            _socket.set_option(typename Socket::keep_alive{true});
        } catch(...) {
            _socket.close();
        }
        _should_reconnect.reset();
    }
}
//------------------------------------------------------------------------------
template <typename Endpoint, typename Socket>
void asio_log_output_stream_base<Endpoint, Socket>::finish_entry() noexcept {
    if(not _socket.is_open()) [[unlikely]] {
        _reconnect();
    }
    _handle_entry();
}
//------------------------------------------------------------------------------
template <typename Endpoint, typename Socket>
void asio_log_output_stream_base<Endpoint, Socket>::finish_stream(
  const memory::const_block& end) noexcept {
    append_to(end, _buffers.next());
    finish_entry();
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
