/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.serialization:iostream;

import std;
import eagine.core.types;
import eagine.core.memory;
import :result;
import :interface;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Deserialization data source backed by an input stream.
/// @ingroup serialization
/// @see ostream_data_sink
export class istream_data_source : public deserializer_data_source {
public:
    /// @brief Constructor setting the source stream.
    istream_data_source(std::istream& in) noexcept
      : _in{in} {}

    auto top(span_size_t req_size) noexcept -> memory::const_block final {
        if(_cur_size < req_size) {
            while(safe_add_gt(_cur_size, req_size, _storage.size())) {
                _storage.resize(safe_add(_storage.size(), _chunk_size()));
            }
            read_from_stream(
              _in,
              head(skip(cover(_storage), _cur_size), req_size - _cur_size));
            _cur_size = safe_add(_cur_size, span_size(_in.gcount()));
        }
        return head(head(view(_storage), _cur_size), req_size);
    }

    void pop(span_size_t del_size) noexcept final {
        if(_cur_size <= del_size) {
            _cur_size = 0;
            if(_storage.size() > _chunk_size()) {
                _storage.resize(_chunk_size());
            }
        } else {
            auto sw = head(cover(_storage), _cur_size);
            memory::copy(skip(sw, del_size), sw);
            _cur_size -= del_size;
        }
        assert(_cur_size >= 0);
    }

private:
    static constexpr auto _chunk_size() noexcept -> span_size_t {
        return span_size(8 * 1024);
    }

    std::istream& _in;
    span_size_t _cur_size{0};
    memory::buffer _storage{};
};
//------------------------------------------------------------------------------
/// @brief Serialization data sink backed by an output stream.
/// @ingroup serialization
/// @see istream_data_source
export class ostream_data_sink : public serializer_data_sink {
public:
    /// @brief Constructor setting the target stream.
    ostream_data_sink(std::ostream& out) noexcept
      : _out{out} {}

    auto remaining_size() noexcept -> span_size_t final {
        return std::numeric_limits<span_size_t>::max();
    }

    using serializer_data_sink::write;

    auto write(memory::const_block blk) noexcept -> serialization_errors final {
        write_to_stream(current(), blk);
        if(current().eof()) [[unlikely]] {
            return {serialization_error_code::too_much_data};
        }
        if(not current().good()) [[unlikely]] {
            return {serialization_error_code::data_sink_error};
        }
        return {};
    }

    auto begin_work() noexcept -> transaction_handle final {
        _subs.emplace();
        return transaction_handle(_subs.size());
    }

    auto commit([[maybe_unused]] const transaction_handle th) noexcept
      -> serialization_errors final {
        assert(th == transaction_handle(_subs.size()));
        std::stringstream top{std::move(_subs.top())};
        _subs.pop();
        current() << top.rdbuf();
        return {};
    }

    void rollback([[maybe_unused]] const transaction_handle th) noexcept final {
        assert(th == transaction_handle(_subs.size()));
        _subs.pop();
    }

    auto finalize() noexcept -> serialization_errors final {
        return {};
    }

private:
    auto current() noexcept -> std::ostream& {
        if(_subs.empty()) {
            return _out;
        }
        return _subs.top();
    }

    std::ostream& _out;
    std::stack<std::stringstream> _subs;
};
} // namespace eagine

