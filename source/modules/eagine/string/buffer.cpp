/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.string:buffer;

import std;
import eagine.core.types;
import eagine.core.memory;

namespace eagine {
//------------------------------------------------------------------------------
export class string_buffer {
public:
    string_buffer(span_size_t size) {
        _buffer.resize(span_size(size), '\0');
    }

    friend auto cover(string_buffer& that) noexcept {
        return memory::cover(that._buffer);
    }

    friend auto view(string_buffer& that) noexcept {
        return memory::view(that._buffer);
    }

    auto clip(span_size_t size) -> auto& {
        _buffer.resize(std_size(size));
        return *this;
    }

    auto get_string() const {
        return std::string{_buffer.data(), _buffer.size()};
    }

    operator std::string() const {
        return get_string();
    }

private:
    std::vector<char> _buffer;
};
//------------------------------------------------------------------------------
} // namespace eagine
