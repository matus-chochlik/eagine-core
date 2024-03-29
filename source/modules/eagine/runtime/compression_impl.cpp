/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>
#if __has_include(<zlib.h>)
#include <zlib.h>
#ifndef EAGINE_USE_ZLIB
#define EAGINE_USE_ZLIB 1
#endif
#else
#ifndef EAGINE_USE_ZLIB
#define EAGINE_USE_ZLIB 0
#endif
#endif

module eagine.core.runtime;
import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.utility;

namespace eagine {
//------------------------------------------------------------------------------
struct data_compressor_intf : abstract<data_compressor_intf> {
    using data_handler = callable_ref<bool(memory::const_block) noexcept>;

    virtual auto default_method() const noexcept -> data_compression_method = 0;

    virtual auto supported_methods() const noexcept
      -> data_compression_methods {
        return {default_method()};
    }

    virtual auto method_header(data_compression_method) const noexcept
      -> memory::const_block = 0;

    auto default_method_header() const noexcept -> memory::const_block {
        return method_header(default_method());
    }

    virtual auto supported_method_from_header(
      const memory::const_block input) const noexcept
      -> std::tuple<data_compression_method, memory::const_block> {
        const auto header{default_method_header()};
        if(are_equal(head(input, header.size()), header)) {
            return {default_method(), skip(input, header.size())};
        }
        return {data_compression_method::unknown, input};
    }

    virtual auto compress_begin(
      const data_compression_method method,
      const data_compression_level level) noexcept -> bool = 0;

    virtual auto compress_next(
      const memory::const_block input,
      const data_handler& handler) noexcept -> bool = 0;

    virtual auto compress_finish(const data_handler& handler) noexcept
      -> bool = 0;

    virtual auto compress(
      const data_compression_method method,
      const memory::const_block input,
      const data_handler& handler,
      const data_compression_level level) noexcept -> bool {
        if(compress_begin(method, level)) {
            compress_next(input, handler);
            return compress_finish(handler);
        }
        return false;
    }

    virtual auto compress(
      const data_compression_method method,
      const memory::const_block input,
      memory::buffer& output,
      const data_compression_level level) noexcept -> memory::const_block {
        auto append = [&](memory::const_block data) {
            append_to(data, output);
            return true;
        };
        if(compress(method, input, {construct_from, append}, level)) {
            return view(output);
        }
        return {};
    }

    virtual auto compress(
      const data_compression_method method,
      const memory::const_block input,
      data_compression_level level) noexcept -> memory::const_block = 0;

    virtual auto decompress_begin(const data_compression_method method) noexcept
      -> bool = 0;

    virtual auto decompress_next(
      const memory::const_block input,
      const data_handler& handler) noexcept -> bool = 0;

    virtual auto decompress_finish(const data_handler& handler) noexcept
      -> bool = 0;

    virtual auto decompress(
      data_compression_method method,
      memory::const_block input,
      const data_handler& handler) noexcept -> bool {
        if(decompress_begin(method)) {
            decompress_next(input, handler);
            return decompress_finish(handler);
        }
        return false;
    }

    virtual auto decompress(
      const data_compression_method method,
      const memory::const_block input,
      memory::buffer& output) noexcept -> memory::const_block {
        auto append = [&](memory::const_block data) {
            append_to(data, output);
            return true;
        };
        if(decompress(method, input, {construct_from, append})) {
            return view(output);
        }
        return {};
    }

    virtual auto decompress(
      const data_compression_method method,
      const memory::const_block input) noexcept -> memory::const_block = 0;
};
//------------------------------------------------------------------------------
class buffered_data_compressor_impl : public data_compressor_intf {
public:
    buffered_data_compressor_impl(memory::buffer_pool& buffers) noexcept
      : _buffers{buffers}
      , _output{buffers.get(0)} {}

    ~buffered_data_compressor_impl() noexcept {
        _buffers.eat(std::move(_output));
    }

    buffered_data_compressor_impl(buffered_data_compressor_impl&&) = delete;
    buffered_data_compressor_impl(const buffered_data_compressor_impl&) = delete;
    auto operator=(buffered_data_compressor_impl&&) = delete;
    auto operator=(const buffered_data_compressor_impl&) = delete;

    using data_compressor_intf::compress;

    auto compress(
      const data_compression_method method,
      const memory::const_block input,
      data_compression_level level) noexcept -> memory::const_block final {
        return compress(method, input, _output.clear(), level);
    }

    using data_compressor_intf::decompress;

    auto decompress(
      const data_compression_method method,
      const memory::const_block input) noexcept -> memory::const_block final {
        return decompress(method, input, _output.clear());
    }

protected:
    memory::buffer_pool& _buffers;

private:
    memory::buffer _output;
};
//------------------------------------------------------------------------------
static auto none_compression_header() noexcept -> memory::const_block {
    static const std::array<byte, 1> header{{0x00U}};
    return view(header);
}
class noop_data_compressor_impl : public buffered_data_compressor_impl {
public:
    using buffered_data_compressor_impl::buffered_data_compressor_impl;

    using data_handler = callable_ref<bool(memory::const_block) noexcept>;

    auto default_method() const noexcept -> data_compression_method final {
        return data_compression_method::none;
    }

    auto method_header([[maybe_unused]] data_compression_method method)
      const noexcept -> memory::const_block final {
        assert(method == default_method());
        return none_compression_header();
    }

    auto compress_begin(
      const data_compression_method method,
      const data_compression_level) noexcept -> bool final {
        return method == default_method();
    }

    auto compress_next(
      const memory::const_block input,
      const data_handler& handler) noexcept -> bool final {
        return handler(input);
    }

    auto compress_finish(const data_handler&) noexcept -> bool final {
        return true;
    }

    auto decompress_begin(const data_compression_method method) noexcept
      -> bool final {
        return method == default_method();
    }

    auto decompress_next(
      const memory::const_block input,
      const data_handler& handler) noexcept -> bool final {
        return handler(input);
    }

    auto decompress_finish(const data_handler&) noexcept -> bool final {
        return true;
    }
};
//------------------------------------------------------------------------------
#if EAGINE_USE_ZLIB
static auto zlib_compression_header() noexcept -> memory::const_block {
    static const std::array<byte, 1> header{{0x01U}};
    return view(header);
}
class zlib_data_compressor_impl : public buffered_data_compressor_impl {
private:
    memory::buffer _temp;
    ::z_stream _zsd{};
    ::z_stream _zsi{};
    int _zd_lvl{Z_DEFAULT_COMPRESSION};
    int _zd_res{Z_STREAM_END};
    int _zi_res{Z_STREAM_END};

    static constexpr auto _translate(const data_compression_level level) noexcept
      -> int {
        switch(level) {
            case data_compression_level::normal:
                break;
            case data_compression_level::none:
                return 0;
            case data_compression_level::lowest:
                return 1;
            case data_compression_level::highest:
                return 9;
        }
        return Z_DEFAULT_COMPRESSION;
    }

public:
    using data_handler = callable_ref<bool(memory::const_block) noexcept>;

    zlib_data_compressor_impl(memory::buffer_pool& buffers)
      : buffered_data_compressor_impl{buffers}
      , _temp{_buffers.get(64 * 1024)} {
        zero(as_bytes(cover_one(_zsd)));
        _zsd.zalloc = nullptr;
        _zsd.zfree = nullptr;
        _zsd.opaque = nullptr;
        zero(as_bytes(cover_one(_zsi)));
        _zsi.zalloc = nullptr;
        _zsi.zfree = nullptr;
        _zsi.opaque = nullptr;
    }

    ~zlib_data_compressor_impl() noexcept {
        _buffers.eat(std::move(_temp));
    }

    zlib_data_compressor_impl(zlib_data_compressor_impl&&) = delete;
    zlib_data_compressor_impl(const zlib_data_compressor_impl&) = delete;
    auto operator=(zlib_data_compressor_impl&&) = delete;
    auto operator=(const zlib_data_compressor_impl&) = delete;

    auto default_method() const noexcept -> data_compression_method final {
        return data_compression_method::zlib;
    }

    auto method_header([[maybe_unused]] data_compression_method method)
      const noexcept -> memory::const_block final {
        assert(method == default_method());
        return zlib_compression_header();
    }

    auto compress_begin(
      const data_compression_method method,
      const data_compression_level level) noexcept -> bool final {
        if(method != default_method()) [[unlikely]] {
            return false;
        }
        if(_zd_res != Z_STREAM_END) [[unlikely]] {
            return false;
        }
        _zd_lvl = _translate(level);

        return true;
    }

    auto compress_handle_data(const data_handler& handler) noexcept -> bool {
        const auto size{_temp.size() - span_size(_zsd.avail_out)};
        if(size > 0) {
            if(handler(head(view(_temp), size))) [[likely]] {
                _zsd.next_out = _temp.data();
                _zsd.avail_out = limit_cast<unsigned>(_temp.size());
            } else {
                return false;
            }
        }
        return true;
    }

    auto compress_next(
      const memory::const_block input,
      const data_handler& handler) noexcept -> bool final {
        _zsd.next_in = const_cast<byte*>(input.data());
        _zsd.avail_in = limit_cast<unsigned>(input.size());
        _zsd.next_out = _temp.data();
        _zsd.avail_out = limit_cast<unsigned>(_temp.size());

        if(_zd_res == Z_STREAM_END) {
            _zd_res = ::deflateInit(&_zsd, _zd_lvl);
        }

        if(_zd_res != Z_OK) [[unlikely]] {
            return false;
        }

        while(true) {
            _zd_res = ::deflate(&_zsd, Z_NO_FLUSH);
            if(_zd_res == Z_BUF_ERROR) {
                if(not compress_handle_data(handler)) {
                    return false;
                }
            } else if((_zd_res == Z_OK) or (_zd_res == Z_STREAM_END)) {
                if(_zsd.avail_out == 0) {
                    if(not compress_handle_data(handler)) {
                        return false;
                    }
                }
                if(_zsd.avail_in == 0) {
                    if(not compress_handle_data(handler)) {
                        return false;
                    }
                    break;
                }
            } else {
                return false;
            }
        }

        return _zd_res == Z_OK;
    }

    auto compress_finish(const data_handler& handler) noexcept -> bool final {
        const auto cleanup_later{finally([this]() {
            ::deflateEnd(&_zsd);
            _zd_res = Z_STREAM_END;
        })};
        while(_zd_res != Z_STREAM_END) {
            _zd_res = ::deflate(&_zsd, Z_FINISH);
            if((_zd_res == Z_STREAM_END) or (_zd_res == Z_OK)) {
                if(not compress_handle_data(handler)) {
                    return false;
                }
                return true;
            } else {
                return false;
            }
        }
        return true;
    }

    auto decompress_begin(const data_compression_method method) noexcept
      -> bool {
        if(method != default_method()) [[unlikely]] {
            return false;
        }
        if(_zi_res != Z_STREAM_END) [[unlikely]] {
            return false;
        }

        return _zi_res != Z_OK;
    }

    auto decompress_handle_data(const data_handler& handler) noexcept -> bool {
        const auto size{_temp.size() - span_size(_zsi.avail_out)};
        if(size > 0) {
            if(handler(head(view(_temp), size))) [[likely]] {
                _zsi.next_out = _temp.data();
                _zsi.avail_out = limit_cast<unsigned>(_temp.size());
            } else {
                return false;
            }
        }
        return true;
    }

    auto decompress_next(
      const memory::const_block input,
      const data_handler& handler) noexcept -> bool {
        _zsi.next_in = const_cast<byte*>(input.data());
        _zsi.avail_in = limit_cast<unsigned>(input.size());
        _zsi.next_out = _temp.data();
        _zsi.avail_out = limit_cast<unsigned>(_temp.size());

        if(_zi_res == Z_STREAM_END) {
            _zi_res = ::inflateInit(&_zsi);
        }

        if(_zi_res != Z_OK) [[unlikely]] {
            return false;
        }

        while(true) {
            _zi_res = ::inflate(&_zsi, Z_NO_FLUSH);
            if(_zi_res == Z_BUF_ERROR) {
                if(not decompress_handle_data(handler)) {
                    return false;
                }
            } else if((_zi_res == Z_OK) or (_zi_res == Z_STREAM_END)) {
                if(_zsi.avail_out == 0) {
                    if(not decompress_handle_data(handler)) {
                        return false;
                    }
                }
                if(_zsi.avail_in == 0) {
                    if(not decompress_handle_data(handler)) {
                        return false;
                    }
                    break;
                }
            } else {
                return false;
            }
        }

        return _zi_res == Z_OK;
    }

    auto decompress_finish(const data_handler& handler) noexcept -> bool {
        const auto cleanup_later{finally([this]() {
            ::inflateEnd(&_zsi);
            _zi_res = Z_STREAM_END;
        })};
        while(_zi_res != Z_STREAM_END) {
            _zi_res = ::inflate(&_zsi, Z_FINISH);
            if((_zi_res == Z_STREAM_END) or (_zi_res == Z_OK)) {
                if(not decompress_handle_data(handler)) {
                    return false;
                }
                return true;
            } else {
                return false;
            }
        }
        return true;
    }
};

auto default_data_compression_method() noexcept -> data_compression_method {
    return data_compression_method::zlib;
}
#else
auto default_data_compression_method() noexcept -> data_compression_method {
    return data_compression_method::none;
}
#endif
//------------------------------------------------------------------------------
static inline auto make_data_compressor_impl(
  memory::buffer_pool& buffers) noexcept
  -> shared_holder<data_compressor_intf> {
#if EAGINE_USE_ZLIB
    return {hold<zlib_data_compressor_impl>, buffers};
#else
    return {hold<noop_data_compressor_impl>, buffers};
#endif
}
//------------------------------------------------------------------------------
static inline auto make_data_compressor_impl_for_method(
  data_compression_method method,
  memory::buffer_pool& buffers) -> shared_holder<data_compressor_intf> {
    switch(method) {
        case data_compression_method::none:
            return {hold<noop_data_compressor_impl>, buffers};
        case data_compression_method::zlib:
#if EAGINE_USE_ZLIB
            return {hold<zlib_data_compressor_impl>, buffers};
#else
            break;
#endif
        case data_compression_method::unknown:
            break;
    }
    throw std::runtime_error("data compression method not available");
}
//------------------------------------------------------------------------------
data_compressor::data_compressor(memory::buffer_pool& buffers) noexcept
  : _impl{make_data_compressor_impl(buffers)} {}
//------------------------------------------------------------------------------
data_compressor::data_compressor(
  data_compression_method method,
  memory::buffer_pool& buffers)
  : _impl{make_data_compressor_impl_for_method(method, buffers)} {}
//------------------------------------------------------------------------------
auto data_compressor::supported_methods() const noexcept
  -> data_compression_methods {
    assert(_impl);
    return _impl->supported_methods();
}
//------------------------------------------------------------------------------
auto data_compressor::default_method() const noexcept
  -> data_compression_method {
    assert(_impl);
    return _impl->default_method();
}
//------------------------------------------------------------------------------
auto data_compression_method_from_header(const memory::const_block data) noexcept
  -> std::tuple<data_compression_method, memory::const_block> {
    const auto header_match = [data](const memory::const_block header) {
        return are_equal(header, head(data, header.size()));
    };
    const auto skip_header = [data](const memory::const_block header) {
        return skip(data, header.size());
    };
#if EAGINE_USE_ZLIB
    if(const auto header{zlib_compression_header()}; header_match(header)) {
        return {data_compression_method::zlib, skip_header(header)};
    }
#endif
    if(const auto header{none_compression_header()}; header_match(header)) {
        return {data_compression_method::none, skip_header(header)};
    }
    return {data_compression_method::unknown, data};
}
//------------------------------------------------------------------------------
auto data_compressor::supported_method_from_header(
  const memory::const_block data) const noexcept
  -> std::tuple<data_compression_method, memory::const_block> {

    assert(_impl);
    return _impl->supported_method_from_header(data);
}
//------------------------------------------------------------------------------
auto data_compressor::compress_begin(
  const data_compression_method method,
  const data_compression_level level) noexcept -> bool {
    assert(_impl);
    return _impl->compress_begin(method, level);
}
//------------------------------------------------------------------------------
auto data_compressor::compress_next(
  const memory::const_block input,
  const data_handler& handler) noexcept -> bool {
    assert(_impl);
    return _impl->compress_next(input, handler);
}
//------------------------------------------------------------------------------
auto data_compressor::compress_finish(const data_handler& handler) noexcept
  -> bool {
    assert(_impl);
    return _impl->compress_finish(handler);
}
//------------------------------------------------------------------------------
auto data_compressor::compress(
  const data_compression_method method,
  const memory::const_block input,
  const data_handler& handler,
  const data_compression_level level) noexcept -> bool {
    assert(_impl);
    return _impl->compress(method, input, handler, level);
}
//------------------------------------------------------------------------------
auto data_compressor::compress(
  const memory::const_block input,
  const data_handler& handler,
  const data_compression_level level) noexcept -> bool {
    assert(_impl);
    handler(_impl->default_method_header());
    return _impl->compress(_impl->default_method(), input, handler, level);
}
//------------------------------------------------------------------------------
auto data_compressor::compress(
  const data_compression_method method,
  const memory::const_block input,
  memory::buffer& output,
  const data_compression_level level) noexcept -> memory::const_block {
    assert(_impl);
    return _impl->compress(method, input, output, level);
}
//------------------------------------------------------------------------------
auto data_compressor::compress(
  const memory::const_block input,
  memory::buffer& output,
  const data_compression_level level) noexcept -> memory::const_block {
    assert(_impl);
    append_to(_impl->default_method_header(), output);
    return _impl->compress(_impl->default_method(), input, output, level);
}
//------------------------------------------------------------------------------
auto data_compressor::compress(
  const data_compression_method method,
  const memory::const_block input,
  const data_compression_level level) noexcept -> memory::const_block {
    assert(_impl);
    return _impl->compress(method, input, level);
}
//------------------------------------------------------------------------------
auto data_compressor::default_compress(
  const memory::const_block input,
  const data_compression_level level) noexcept -> memory::const_block {
    assert(_impl);
    return _impl->compress(_impl->default_method(), input, level);
}
//------------------------------------------------------------------------------
auto data_compressor::decompress_begin(
  const data_compression_method method) noexcept -> bool {
    assert(_impl);
    return _impl->decompress_begin(method);
}
//------------------------------------------------------------------------------
auto data_compressor::decompress_next(
  const memory::const_block input,
  const data_handler& handler) noexcept -> bool {
    assert(_impl);
    return _impl->decompress_next(input, handler);
}
//------------------------------------------------------------------------------
auto data_compressor::decompress_finish(const data_handler& handler) noexcept
  -> bool {
    assert(_impl);
    return _impl->decompress_finish(handler);
}
//------------------------------------------------------------------------------
auto data_compressor::decompress(
  const data_compression_method method,
  const memory::const_block input,
  const data_handler& handler) noexcept -> bool {
    assert(_impl);
    return _impl->decompress(method, input, handler);
}
//------------------------------------------------------------------------------
auto data_compressor::decompress(
  const memory::const_block data,
  const data_handler& handler) noexcept -> bool {
    assert(_impl);
    const auto [method, input] = _impl->supported_method_from_header(data);
    return _impl->decompress(method, input, handler);
}
//------------------------------------------------------------------------------
auto data_compressor::decompress(
  const data_compression_method method,
  const memory::const_block input,
  memory::buffer& output) noexcept -> memory::const_block {
    if(input) {
        assert(_impl);
        return _impl->decompress(method, input, output);
    }
    return {};
}
//------------------------------------------------------------------------------
auto data_compressor::decompress(
  const memory::const_block data,
  memory::buffer& output) noexcept -> memory::const_block {
    if(data) {
        assert(_impl);
        const auto [method, input] = _impl->supported_method_from_header(data);
        return _impl->decompress(method, input, output);
    }
    return {};
}
//------------------------------------------------------------------------------
auto data_compressor::decompress(
  const data_compression_method method,
  const memory::const_block input) noexcept -> memory::const_block {
    if(input) {
        assert(_impl);
        return _impl->decompress(method, input);
    }
    return {};
}
//------------------------------------------------------------------------------
auto data_compressor::default_decompress(
  const memory::const_block input) noexcept -> memory::const_block {
    if(input) {
        assert(_impl);
        return _impl->decompress(_impl->default_method(), input);
    }
    return {};
}
//------------------------------------------------------------------------------
// stream_compression
//------------------------------------------------------------------------------
auto stream_compression::next(
  const memory::const_block input,
  data_compression_level level) noexcept -> stream_compression& {
    if(not _started) {
        if(_compressor.compress_begin(_method, level)) {
            _started = true;
        } else {
            _failed = true;
        }
    }
    if(_started) {
        if(input.empty() or not _compressor.compress_next(input, _handler)) {
            _finished = true;
            _failed = not _compressor.compress_finish(_handler);
        }
    }
    return *this;
}
//------------------------------------------------------------------------------
auto stream_compression::finish() noexcept -> stream_compression& {
    if(_started) {
        if(not _finished) {
            _finished = true;
            _failed = not _compressor.compress_finish(_handler);
        }
    }
    return *this;
}
//------------------------------------------------------------------------------
// stream_decompression
//------------------------------------------------------------------------------
auto stream_decompression::next(const memory::const_block input) noexcept
  -> stream_decompression& {
    if(not _started) {
        if(_compressor.decompress_begin(_method)) {
            _started = true;
        } else {
            _failed = true;
        }
    }
    if(_started) {
        if(input.empty() or not _compressor.decompress_next(input, _handler)) {
            _finished = true;
            _failed = not _compressor.decompress_finish(_handler);
        }
    }
    return *this;
}
//------------------------------------------------------------------------------
auto stream_decompression::finish() noexcept -> stream_decompression& {
    if(_started) {
        if(not _finished) {
            _finished = true;
            _failed = not _compressor.decompress_finish(_handler);
        }
    }
    return *this;
}
//------------------------------------------------------------------------------
} // namespace eagine
