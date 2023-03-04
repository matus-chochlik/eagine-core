/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.string:base64dump;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.utility;
import eagine.core.valid_if;
import :base64;
import std;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Class for encoding byte blocks into base64 format.
/// @ingroup logging
/// @see bindump
/// @see hexdump
///
/// This class stores a memory block and can be used to write the content of the
/// block into an standard output stream in base64-encoding.
export class base64dump {
public:
    /// @brief Construction initializing the dumped memory block.
    base64dump(const memory::const_block mb) noexcept
      : _mb{mb} {}

    /// @brief Converts the block to base64 and calls the specified function on each char.
    template <typename Function>
    void apply(Function function) const {
        span_size_t i = 0;
        do_dissolve_bits(
          make_span_getter(i, _mb),
          [&function](byte b) {
              if(const auto opt_c{make_base64_encode_transform()(b)}) {
                  function(extract(opt_c));
                  return true;
              }
              return false;
          },
          6);
    }

    /// @brief Operator for writing instances of base64dump to standard output streams.
    friend auto operator<<(std::ostream& out, const base64dump& src)
      -> std::ostream& {
        span_size_t i = 0;
        do_dissolve_bits(
          make_span_getter(i, src._mb),
          [&out](byte b) {
              if(const auto opt_c{make_base64_encode_transform()(b)}) {
                  out << extract(opt_c);
                  return true;
              }
              return false;
          },
          6);
        return out;
    }

private:
    memory::const_block _mb;
};
//------------------------------------------------------------------------------
} // namespace eagine
