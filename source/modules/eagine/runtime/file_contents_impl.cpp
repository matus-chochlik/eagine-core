/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cstdio>

module eagine.core.runtime;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.string;
import eagine.core.utility;
import eagine.core.container;

namespace eagine {
//------------------------------------------------------------------------------
// temporary_chunk_storage_impl
//------------------------------------------------------------------------------
class temporary_chunk_storage_impl {
public:
    auto empty() noexcept -> bool;
    void add_chunk(memory::const_block) noexcept;
    void for_each_chunk(callable_ref<void(memory::const_block)>);
    void clear() noexcept;

private:
    void _write(memory::const_block) noexcept;

    memory::buffer _buffer;
    std::unique_ptr<::FILE, int (*)(::FILE*)> _store{
      std::tmpfile(),
      &std::fclose};
    std::size_t _done{0U};
};
//------------------------------------------------------------------------------
void temporary_chunk_storage_impl::_write(memory::const_block blk) noexcept {
    _done += std::fwrite(blk.data(), 1, blk.std_size(), _store.get());
}
//------------------------------------------------------------------------------
auto temporary_chunk_storage_impl::empty() noexcept -> bool {
    return _done == 0U;
}
//------------------------------------------------------------------------------
void temporary_chunk_storage_impl::add_chunk(
  memory::const_block chunk) noexcept {
    if(_store) {
        std::array<byte, 8> header{};
        using multi_byte::code_point;

        if(const ok size_cp{limit_cast<code_point>(chunk.size())}) {
            using multi_byte::required_sequence_length;
            if(const ok size_len{required_sequence_length(size_cp)}) {
                if(size_len <= header.size()) {
                    using multi_byte::encode_code_point;
                    if(const ok skip_len{
                         encode_code_point(size_cp, memory::cover(header))}) {
                        _write(head(memory::view(header), span_size(skip_len)));
                        _write(chunk);
                    }
                }
            }
        }
    }
}
//------------------------------------------------------------------------------
void temporary_chunk_storage_impl::for_each_chunk(
  callable_ref<void(memory::const_block)> handler) {
    if(_store) {
        std::rewind(_store.get());
        static_vector<byte, 8> header{};
        while(not header.full()) {
            byte b{};
            if(std::fread(&b, 1, 1, _store.get()) != 1) {
                break;
            }
            header.push_back(b);
            if(const auto l{multi_byte::decode_sequence_length(view(header))}) {
                if(header.size() == *l) {
                    header.emplace_back();
                    const auto [skip_len, data_len]{
                      multi_byte::do_decode_length_and_code_point(
                        view(header))};
                    if(skip_len and data_len) {
                        header.clear();
                        _buffer.resize(*data_len);
                        auto todo{cover(_buffer)};
                        while(not todo.empty()) {
                            const auto done{std::fread(
                              todo.data(), 1, todo.size(), _store.get())};
                            if(not done) {
                                return;
                            }
                            todo = skip(todo, span_size(done));
                        }
                        handler(view(_buffer));
                    }
                }
            }
        }
    }
}
//------------------------------------------------------------------------------
void temporary_chunk_storage_impl::clear() noexcept {
    _store.reset(std::tmpfile());
}
//------------------------------------------------------------------------------
// temporary_chunk_storage
//------------------------------------------------------------------------------
temporary_chunk_storage::temporary_chunk_storage() noexcept
  : _impl{hold<temporary_chunk_storage_impl>} {}
//------------------------------------------------------------------------------
auto temporary_chunk_storage::empty() const noexcept -> bool {
    return _impl->empty();
}
//------------------------------------------------------------------------------
auto temporary_chunk_storage::add_chunk(memory::const_block chunk) noexcept
  -> temporary_chunk_storage& {
    _impl->add_chunk(chunk);
    return *this;
}
//------------------------------------------------------------------------------
auto temporary_chunk_storage::for_each_chunk(
  callable_ref<void(memory::const_block)> handler) -> temporary_chunk_storage& {
    _impl->for_each_chunk(handler);
    return *this;
}
//------------------------------------------------------------------------------
auto temporary_chunk_storage::clear() noexcept -> temporary_chunk_storage& {
    _impl->clear();
    return *this;
}
//------------------------------------------------------------------------------
// helper functions
//------------------------------------------------------------------------------
static inline auto do_read_stream_data(
  std::istream& input,
  memory::buffer& dest) noexcept -> bool {
    try {
        const std::ios_base::iostate oldexc = input.exceptions();
        const auto reset_exc =
          finally([&input, oldexc] { input.exceptions(oldexc); });
        input.exceptions(std::ios::failbit | std::ios::badbit);

        input.seekg(0, std::ios::end);
        dest.resize(memory::buffer::size_type(input.tellg()));
        input.seekg(0, std::ios::beg);
        input.read(
          static_cast<char*>(dest.addr()),
          static_cast<std::streamsize>(dest.size()));
        return true;
    } catch(const std::ios_base::failure&) {
        std::vector<char> temp;
        temp.insert(
          temp.begin(),
          std::istreambuf_iterator<char>(input),
          std::istreambuf_iterator<char>());
        if(not input.bad()) {
            dest.resize(span_size(temp.size()));
            memory::copy(as_bytes(cover(temp)), dest);
            return true;
        }
    }
    return false;
}
//------------------------------------------------------------------------------
auto read_stream_data(std::istream& input, memory::buffer& dest) noexcept
  -> bool {
    return do_read_stream_data(input, dest);
}
//------------------------------------------------------------------------------
auto read_file_data(const string_view path, memory::buffer& dest) noexcept
  -> bool {
    std::ifstream file;
    file.open(c_str(path), std::ios::in | std::ios::binary);

    return do_read_stream_data(file, dest);
}
//------------------------------------------------------------------------------
class buffered_file_contents : public file_contents_intf {
private:
    memory::buffer _buf;

public:
    buffered_file_contents(const string_view path) {
        read_file_data(path, _buf);
    }

    auto block() noexcept -> memory::const_block override {
        return _buf;
    }
};
//------------------------------------------------------------------------------
static inline auto make_file_contents_impl(const string_view path)
  -> shared_holder<file_contents_intf> {
    try {
        return {hold<buffered_file_contents>, path};
    } catch(const std::system_error&) {
        return {};
    }
}
//------------------------------------------------------------------------------
// file_contents::file_contents
//------------------------------------------------------------------------------
file_contents::file_contents(const string_view path)
  : _impl{make_file_contents_impl(path)} {}
//------------------------------------------------------------------------------
} // namespace eagine
