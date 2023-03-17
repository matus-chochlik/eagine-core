/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module eagine.core.runtime;

import eagine.core.memory;
import :input_data;
import std;

namespace eagine {
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
inline auto make_file_contents_impl(const string_view path)
  -> std::shared_ptr<file_contents_intf> {
    try {
        return std::make_shared<buffered_file_contents>(path);
    } catch(const std::system_error&) {
        return {};
    }
}
//------------------------------------------------------------------------------
// file_contents::file_contents
//------------------------------------------------------------------------------
file_contents::file_contents(const string_view path)
  : _pimpl{make_file_contents_impl(path)} {}
//------------------------------------------------------------------------------
} // namespace eagine
