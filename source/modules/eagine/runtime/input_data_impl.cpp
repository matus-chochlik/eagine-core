/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module eagine.core.runtime;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.string;
import eagine.core.utility;
import <fstream>;
import <vector>;

namespace eagine {
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
} // namespace eagine
