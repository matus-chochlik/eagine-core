/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

// #ifdef __clang__
// EAGINE_DIAG_PUSH()
// EAGINE_DIAG_OFF(disabled - macro - expansion)
// EAGINE_DIAG_OFF(covered - switch - default)
// EAGINE_DIAG_OFF(zero - as - null - pointer - constant)
// EAGINE_DIAG_OFF(shorten - 64 - to - 32)
// EAGINE_DIAG_OFF(suggest - destructor - override)
// EAGINE_DIAG_OFF(suggest - override)
// EAGINE_DIAG_OFF(deprecated)
// EAGINE_DIAG_OFF(shadow)
// #endif

#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/local/stream_protocol.hpp>
#include <asio/streambuf.hpp>
#include <asio/write.hpp>

// #ifdef __clang__
// EAGINE_DIAG_POP()
// #endif

module eagine.core.logging;

import eagine.core.memory;
import eagine.core.string;
import eagine.core.utility;
import :backend;
import :ostream_backend;
import <iostream>;
import <mutex>;

namespace eagine {
//------------------------------------------------------------------------------
class asio_local_ostream_log_connection {
public:
    asio_local_ostream_log_connection(const string_view addr_str)
      : _endpoint{c_str(_fix_addr(addr_str))}
      , _socket{_context}
      , _out{&_buffer} {
        _socket.connect(_endpoint);
    }

    auto out() noexcept -> std::ostream& {
        if(_socket.is_open()) [[likely]] {
            return _out;
        }
        return std::clog;
    }

protected:
    void _flush(bool always) {
        try {
            if(always || _buffer.size() > 2048) {
                if(_socket.is_open()) [[likely]] {
                    const auto done = asio::write(_socket, _buffer);
                    _buffer.consume(done);
                }
            }
        } catch(...) {
        }
    }

private:
    static auto _fix_addr(const string_view addr_str) noexcept -> string_view {
        return addr_str ? addr_str : string_view{"/tmp/eagine-xmllog"};
    }

    asio::io_context _context{};
    asio::local::stream_protocol::endpoint _endpoint{};
    asio::local::stream_protocol::socket _socket;
    asio::streambuf _buffer{};
    std::ostream _out;
};
//------------------------------------------------------------------------------
class asio_tcpipv4_ostream_log_connection {
public:
    asio_tcpipv4_ostream_log_connection(const string_view addr_str)
      : _addr{_fix_addr(addr_str)}
      , _resolver{_context}
      , _socket{_context}
      , _out{&_buffer} {
        for(const auto& endpt :
            _resolver.resolve(std::get<0>(_addr), std::get<1>(_addr))) {
            _socket.connect(endpt);
            break;
        }
    }

    auto out() noexcept -> std::ostream& {
        if(_socket.is_open()) [[likely]] {
            return _out;
        }
        return std::clog;
    }

protected:
    void _flush(bool always) {
        try {
            if(always || _buffer.size() > 2048) {
                if(_socket.is_open()) [[likely]] {
                    const auto done = asio::write(_socket, _buffer);
                    _buffer.consume(done);
                }
            }
        } catch(...) {
        }
    }

private:
    static auto _fix_addr(const string_view addr_str)
      -> std::tuple<std::string, std::string> {
        auto [hostname, port_str] = split_by_last(addr_str, string_view(":"));
        return {
          hostname ? to_string(hostname) : std::string("localhost"),
          port_str ? to_string(port_str) : std::string("34917")};
    }
    std::tuple<std::string, std::string> _addr;

    asio::io_context _context{};
    asio::ip::tcp::resolver _resolver;
    asio::ip::tcp::socket _socket;
    asio::streambuf _buffer{};
    std::ostream _out;
};
//------------------------------------------------------------------------------
template <typename Connection, typename Lockable>
class asio_ostream_log_backend final
  : private Connection
  , public ostream_log_backend<Lockable> {

    using base = ostream_log_backend<Lockable>;

public:
    asio_ostream_log_backend(
      const string_view addr_str,
      const log_stream_info& info)
      : Connection{addr_str}
      , ostream_log_backend<Lockable>{Connection::out(), info} {}

    asio_ostream_log_backend(const log_stream_info& info)
      : asio_ostream_log_backend{string_view{}, info} {}

    asio_ostream_log_backend(asio_ostream_log_backend&&) = delete;
    asio_ostream_log_backend(const asio_ostream_log_backend&) = delete;
    auto operator=(asio_ostream_log_backend&&) = delete;
    auto operator=(const asio_ostream_log_backend&) = delete;

    ~asio_ostream_log_backend() noexcept final {
        this->finish_log();
    }

    void flush(bool always) noexcept final {
        std::lock_guard<Lockable> lock{_flush_lockable};
        Connection::_flush(always);
    }

private:
    Lockable _flush_lockable;
};
//------------------------------------------------------------------------------
template <typename Lockable>
using asio_local_ostream_log_backend =
  asio_ostream_log_backend<asio_local_ostream_log_connection, Lockable>;

template <typename Lockable = std::mutex>
using asio_tcpipv4_ostream_log_backend =
  asio_ostream_log_backend<asio_tcpipv4_ostream_log_connection, Lockable>;
//------------------------------------------------------------------------------
auto make_asio_local_ostream_log_backend_mutex(const log_stream_info& info)
  -> std::unique_ptr<logger_backend> {
    return std::make_unique<asio_local_ostream_log_backend<std::mutex>>(info);
}

auto make_asio_local_ostream_log_backend_spinlock(const log_stream_info& info)
  -> std::unique_ptr<logger_backend> {
    return std::make_unique<asio_local_ostream_log_backend<spinlock>>(info);
}

auto make_asio_local_ostream_log_backend_mutex(
  string_view addr,
  const log_stream_info& info) -> std::unique_ptr<logger_backend> {
    return std::make_unique<asio_local_ostream_log_backend<std::mutex>>(
      addr, info);
}

auto make_asio_local_ostream_log_backend_spinlock(
  string_view addr,
  const log_stream_info& info) -> std::unique_ptr<logger_backend> {
    return std::make_unique<asio_local_ostream_log_backend<spinlock>>(
      addr, info);
}

auto make_asio_tcpipv4_ostream_log_backend_mutex(
  string_view addr,
  const log_stream_info& info) -> std::unique_ptr<logger_backend> {
    return std::make_unique<asio_tcpipv4_ostream_log_backend<std::mutex>>(
      addr, info);
}

auto make_asio_tcpipv4_ostream_log_backend_spinlock(
  string_view addr,
  const log_stream_info& info) -> std::unique_ptr<logger_backend> {
    return std::make_unique<asio_tcpipv4_ostream_log_backend<spinlock>>(
      addr, info);
}
//------------------------------------------------------------------------------
} // namespace eagine
