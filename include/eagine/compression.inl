/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#include <array>
#include <eagine/memory/span_algo.hpp>
#if EAGINE_USE_ZLIB
#include <eagine/span.hpp>
#include <zlib.h>
#endif

namespace eagine {
//------------------------------------------------------------------------------
#if EAGINE_USE_ZLIB
class data_compressor_impl {
private:
	memory::buffer _buff{};
	std::array<byte, 16 * 1024> _temp{};
	::z_stream _zsd{};
	::z_stream _zsi{};

	static constexpr auto _translate(data_compression_level level) noexcept
	  -> int {
		switch(level) {
			case data_compression_level::none:
				return 0;
			case data_compression_level::lowest:
				return 1;
			case data_compression_level::normal:
				break;
			case data_compression_level::highest:
				return 9;
		}
		return Z_DEFAULT_COMPRESSION;
	}

public:
	using data_handler = callable_ref<bool(memory::const_block)>;

	data_compressor_impl() noexcept {
		zero(as_bytes(cover_one(_zsd)));
		_zsd.zalloc = nullptr;
		_zsd.zfree = nullptr;
		_zsd.opaque = nullptr;
		zero(as_bytes(cover_one(_zsi)));
		_zsi.zalloc = nullptr;
		_zsi.zfree = nullptr;
		_zsi.opaque = nullptr;
	}

	auto compress(memory::const_block input,
	  const data_handler& handler,
	  data_compression_level level) -> bool {
		_zsd.next_in = const_cast<byte*>(input.data());
		_zsd.avail_in = input.size();
		_zsd.next_out = _temp.data();
		_zsd.avail_out = _temp.size();

		auto append = [&](span_size_t size) -> bool {
			if(handler(head(view(_temp), size))) {
				_zsd.next_out = _temp.data();
				_zsd.avail_out = _temp.size();
				return true;
			}
			return false;
		};

		int zres{};
		if((zres = ::deflateInit(&_zsd, _translate(level))) != Z_OK) {
			return false;
		}

		while(_zsd.avail_in > 0) {
			if((zres = ::deflate(&_zsd, Z_NO_FLUSH)) != Z_OK) {
				if(zres != Z_STREAM_END) {
					return false;
				}
			}
			if(_zsd.avail_out == 0) {
				if(!append(span_size(_temp.size()))) {
					return false;
				}
			}
		}

		while(zres == Z_OK) {
			if(_zsd.avail_out == 0) {
				if(!append(span_size(_temp.size()))) {
					return false;
				}
			}
			zres = ::deflate(&_zsd, Z_FINISH);
		}

		if((zres != Z_OK) && (zres != Z_STREAM_END)) {
			return false;
		}

		if(!append(span_size(_temp.size() - _zsd.avail_out))) {
			return false;
		}
		::deflateEnd(&_zsd);

		return true;
	}

	auto compress(memory::const_block input,
	  memory::buffer& output,
	  data_compression_level level) -> memory::const_block {
		auto append = [&](memory::const_block blk) {
			const auto sk = output.size();
			output.enlarge_by(blk.size());
			copy(blk, skip(cover(output), sk));
			return true;
		};

		output.resize(1);
		cover(output).front() = 0x01U;

		if(compress(input, data_handler(construct_from, append), level)) {
			return view(output);
		}
		return {};
	}

	auto compress(memory::const_block input, data_compression_level level)
	  -> memory::const_block {
		return compress(input, _buff, level);
	}

	auto decompress(memory::const_block input, const data_handler& handler)
	  -> bool {
		if(!input) {
			return false;
		}
		if(input.front() != 0x01U) {
			return false;
		}

		input = skip(input, 1);

		_zsi.next_in = const_cast<byte*>(input.data());
		_zsi.avail_in = input.size();
		_zsi.next_out = _temp.data();
		_zsi.avail_out = _temp.size();

		auto append = [&](span_size_t size) -> bool {
			if(handler(head(view(_temp), size))) {
				_zsi.next_out = _temp.data();
				_zsi.avail_out = _temp.size();
				return true;
			}
			return false;
		};

		int zres{};
		if((zres = ::inflateInit(&_zsi)) != Z_OK) {
			return false;
		}

		while(_zsi.avail_in > 0) {
			if((zres = ::inflate(&_zsi, Z_NO_FLUSH)) != Z_OK) {
				if(zres != Z_STREAM_END) {
					return false;
				}
			}
			if(_zsi.avail_out == 0) {
				if(!append(span_size(_temp.size()))) {
					return false;
				}
			}
		}

		while(zres == Z_OK) {
			if(_zsd.avail_out == 0) {
				if(!append(span_size(_temp.size()))) {
					return false;
				}
			}
			zres = ::inflate(&_zsd, Z_FINISH);
		}

		if((zres != Z_OK) && (zres != Z_STREAM_END)) {
			return false;
		}

		if(!append(span_size(_temp.size() - _zsi.avail_out))) {
			return false;
		}
		::inflateEnd(&_zsi);

		return true;
	}

	auto decompress(memory::const_block input, memory::buffer& output)
	  -> memory::const_block {
		auto append = [&](memory::const_block blk) {
			const auto sk = output.size();
			output.enlarge_by(blk.size());
			copy(blk, skip(cover(output), sk));
			_zsi.next_out = _temp.data();
			_zsi.avail_out = _temp.size();
			return true;
		};
		output.clear();

		if(decompress(input, data_handler(construct_from, append))) {
			return view(output);
		}
		return {};
	}

	auto decompress(memory::const_block input) -> memory::const_block {
		if(input.front() == 0x00U) {
			return skip(input, 1);
		}
		return decompress(input, _buff);
	}
};
#else
class data_compressor_impl {
public:
	using data_handler = callable_ref<bool(memory::const_block)>;

	auto compress(
	  memory::const_block, const data_handler&, data_compression_level)
	  -> bool {
		return false;
	}

	auto compress(memory::const_block input,
	  memory::buffer& output,
	  data_compression_level level) -> memory::const_block {
		EAGINE_MAYBE_UNUSED(level);
		output.resize(input.size() + 1);
		copy(input, skip(cover(output), 1));
		cover(output).front() = 0x00U;
		return view(output);
	}

	auto compress(memory::const_block block, data_compression_level level)
	  -> memory::const_block {
		return compress(block, _buff, level);
	}

	auto decompress(memory::const_block, const data_handler&) -> bool {
		return false;
	}

	auto decompress(memory::const_block input, memory::buffer& output) const
	  -> memory::const_block {
		if(input.front() == 0x00U) {
			output.resize(input.size() - 1);
			copy(skip(input, 1), cover(output));
			return view(output);
		}
		return {};
	}

	auto decompress(memory::const_block input) const -> memory::const_block {
		if(input.front() == 0x00U) {
			return skip(input, 1);
		}
		return {};
	}

private:
	memory::buffer _buff{};
};
#endif
//------------------------------------------------------------------------------
static inline auto make_data_compressor_impl() {
	return std::make_shared<data_compressor_impl>();
}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
data_compressor::data_compressor()
  : _pimpl{make_data_compressor_impl()} {
}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
auto data_compressor::compress(memory::const_block input,
  const data_handler& handler,
  data_compression_level level) -> bool {
	EAGINE_ASSERT(_pimpl);
	return _pimpl->compress(input, handler, level);
}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
auto data_compressor::compress(memory::const_block input,
  memory::buffer& output,
  data_compression_level level) -> memory::const_block {
	EAGINE_ASSERT(_pimpl);
	if(auto result{_pimpl->compress(input, output, level)}) {
		return result;
	}
	output.resize(input.size() + 1);
	copy(input, skip(cover(output), 1));
	cover(output).front() = 0x00U;
	return view(output);
}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
auto data_compressor::compress(memory::const_block input,
  data_compression_level level) -> memory::const_block {
	EAGINE_ASSERT(_pimpl);
	if(auto result{_pimpl->compress(input, level)}) {
		return result;
	}
	return {};
}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
auto data_compressor::decompress(
  memory::const_block input, const data_handler& handler) -> bool {
	EAGINE_ASSERT(_pimpl);
	return _pimpl->decompress(input, handler);
}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
auto data_compressor::decompress(
  memory::const_block input, memory::buffer& output) -> memory::const_block {
	if(input) {
		EAGINE_ASSERT(_pimpl);
		if(auto result{_pimpl->decompress(input, output)}) {
			return result;
		}
	}
	return {};
}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
auto data_compressor::decompress(memory::const_block input)
  -> memory::const_block {
	if(input) {
		EAGINE_ASSERT(_pimpl);
		if(auto result{_pimpl->decompress(input)}) {
			return result;
		}
	}
	return {};
}
//------------------------------------------------------------------------------
} // namespace eagine
