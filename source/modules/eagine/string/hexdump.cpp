/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.string:hexdump;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.utility;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Class for encoding byte blocks into hexdump-like format.
/// @ingroup logging
/// @see bindump
/// @see base64dump
///
/// This class stores a memory block and can be used to write the content of the
/// block into an standard output stream in hexadecimal format similar to the
/// output of the command-line hexdump tool.
export class hexdump {
public:
    /// @brief Construction initializing the dumped memory block.
    hexdump(memory::const_block mb) noexcept
      : _mb{mb} {}

    /// @brief Alias for source byte getter callable.
    using byte_getter = callable_ref<std::optional<byte>()>;

    /// @brief Alias for destination encoded char putter callable.
    using char_putter = callable_ref<bool(char)>;

    /// @brief Uses get_byte to read input bytes, encodes them and calls put_char.
    static void apply(const byte_getter get_byte, const char_putter put_char);

    /// @brief Write this hexdump object to output stream.
    auto to_stream(std::ostream&) const noexcept -> std::ostream&;

    /// @brief Operator for writing instances of hexdump to standard output streams.
    /// @see to_stream
    friend auto operator<<(std::ostream& out, const hexdump& hd)
      -> std::ostream& {
        return hd.to_stream(out);
    }

private:
    memory::const_block _mb{};
};
//------------------------------------------------------------------------------
} // namespace eagine

