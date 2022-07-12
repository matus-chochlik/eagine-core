/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.serialization:type_sudoku;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.utility;
import :data_buffer;
import :read;
import :write;

namespace eagine {
//------------------------------------------------------------------------------
export template <identifier_t Sid, unsigned S, bool T, typename Selector>
struct get_serialize_buffer_size<Sid, basic_sudoku_board<S, T>, Selector>
  : get_serialize_buffer_size<
      Sid,
      typename basic_sudoku_board<S, T>::blocks_type,
      Selector> {

    static constexpr auto get(
      string_view s,
      const basic_sudoku_board<S, T>& v) noexcept {
        return get_serialize_buffer_size<
          Sid,
          typename basic_sudoku_board<S, T>::blocks_type,
          Selector>::get(s, v.blocks());
    }
};
//------------------------------------------------------------------------------
export template <unsigned S, bool T>
struct serializer<basic_sudoku_board<S, T>>
  : common_serializer<basic_sudoku_board<S, T>> {
    template <typename Backend>
    auto write(const basic_sudoku_board<S, T>& value, Backend& backend) const {
        return _serializer.write(value._blocks, backend);
    }

private:
    serializer<typename basic_sudoku_board<S, T>::blocks_type> _serializer{};
};
//------------------------------------------------------------------------------
export template <unsigned S, bool T>
struct deserializer<basic_sudoku_board<S, T>>
  : common_deserializer<basic_sudoku_board<S, T>> {
    template <typename Backend>
    auto read(basic_sudoku_board<S, T>& value, Backend& backend) const {
        const auto errors{_deserializer.read(value._blocks, backend)};
        return errors;
    }

private:
    deserializer<typename basic_sudoku_board<S, T>::blocks_type> _deserializer{};
};
//------------------------------------------------------------------------------
} // namespace eagine

