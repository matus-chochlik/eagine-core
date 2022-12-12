/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.logging:ostream_backend;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.string;
import eagine.core.identifier;
import eagine.core.reflection;
import eagine.core.utility;
import :backend;
import :fast_xml_backend;
import <string_view>;
import <ostream>;

namespace eagine {
//------------------------------------------------------------------------------
export template <typename Lockable>
class ostream_log_backend
  : public fast_xml_log_backend<Lockable, ostream_log_backend<Lockable>> {
    using base = fast_xml_log_backend<Lockable, ostream_log_backend<Lockable>>;

public:
    ostream_log_backend(std::ostream& out, const log_stream_info& info) noexcept
      : base{info}
      , _out{out} {}

    auto type_id() noexcept -> identifier final {
        return "OutStream";
    }

    void write(const std::string_view str) const {
        _out.write(str.data(), static_cast<std::streamsize>(str.size()));
    }

    virtual void flush([[maybe_unused]] bool always) noexcept {}

private:
    std::ostream& _out;
};
//------------------------------------------------------------------------------
} // namespace eagine
