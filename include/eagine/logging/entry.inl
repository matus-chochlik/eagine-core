/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

namespace eagine {
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
auto log_entry::arg(
  const identifier name,
  const identifier tag,
  const span<const std::int64_t> values) noexcept -> log_entry& {

    if(_backend) {
        _args.add([=](logger_backend& backend) {
            for(auto value : values) {
                backend.add_integer(name, tag, value);
            }
        });
    }
    return *this;
}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
auto log_entry::arg(
  const identifier name,
  const identifier tag,
  const span<const std::int32_t> values) noexcept -> log_entry& {
    if(_backend) {
        _args.add([=](logger_backend& backend) {
            for(auto value : values) {
                backend.add_integer(name, tag, value);
            }
        });
    }
    return *this;
}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
auto log_entry::arg(
  identifier name,
  identifier tag,
  span<const std::int16_t> values) noexcept -> log_entry& {
    if(_backend) {
        _args.add([=](logger_backend& backend) {
            for(auto value : values) {
                backend.add_integer(name, tag, value);
            }
        });
    }
    return *this;
}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
auto log_entry::arg(
  identifier name,
  identifier tag,
  span<const std::uint64_t> values) noexcept -> log_entry& {
    if(_backend) {
        _args.add([=](logger_backend& backend) {
            for(auto value : values) {
                backend.add_unsigned(name, tag, value);
            }
        });
    }
    return *this;
}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
auto log_entry::arg(
  identifier name,
  identifier tag,
  span<const std::uint32_t> values) noexcept -> log_entry& {
    if(_backend) {
        _args.add([=](logger_backend& backend) {
            for(auto value : values) {
                backend.add_unsigned(name, tag, value);
            }
        });
    }
    return *this;
}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
auto log_entry::arg(
  identifier name,
  identifier tag,
  span<const std::uint16_t> values) noexcept -> log_entry& {
    if(_backend) {
        _args.add([=](logger_backend& backend) {
            for(auto value : values) {
                backend.add_unsigned(name, tag, value);
            }
        });
    }
    return *this;
}
//------------------------------------------------------------------------------
EAGINE_LIB_FUNC
auto log_entry::arg(
  identifier name,
  identifier tag,
  span<const float> values) noexcept -> log_entry& {
    if(_backend) {
        for(auto value : values) {
            _args.add([=](logger_backend& backend) {
                backend.add_float(name, tag, value);
            });
        }
    }
    return *this;
}

//------------------------------------------------------------------------------
} // namespace eagine
