/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.serialization:size_and_data;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.string;
import eagine.core.valid_if;

namespace eagine {
//------------------------------------------------------------------------------
// (size+content, content, data, padding)
export [[nodiscard]] auto pad_and_store_data_with_size(
  const memory::const_block src,
  const span_size_t padded_size,
  memory::block dst) noexcept
  -> std::tuple<memory::block, memory::block, memory::block, memory::block> {
    assert(src.size() <= padded_size);
    using multi_byte::code_point;

    if(const ok size_cp{limit_cast<code_point>(src.size())}) [[likely]] {
        using multi_byte::required_sequence_length;
        if(const ok size_len{required_sequence_length(size_cp)}) [[likely]] {
            if(size_len + padded_size <= dst.size()) {
                using multi_byte::encode_code_point;
                if(ok skip_len{encode_code_point(size_cp, dst)}) [[likely]] {
                    const auto skipped{skip(dst, span_size(skip_len))};
                    const auto data{head(skipped, src.size())};
                    copy(src, data);
                    auto padding{head(
                      skip(dst, span_size(skip_len) + src.size()),
                      padded_size - src.size())};
                    auto fill = padding;
                    while(not fill.empty()) {
                        scramble(head(src, fill.size()), fill);
                        fill = skip(fill, src.size());
                    }
                    return {
                      head(dst, span_size(skip_len) + padded_size),
                      head(skipped, padded_size),
                      data,
                      padding};
                } else {
                    multi_byte::encode_nil(dst);
                }
            } else {
                multi_byte::encode_nil(dst);
            }
        }
    }

    return {{}, {}, {}, {}};
}
//------------------------------------------------------------------------------
auto get_skip_and_data_length(const memory::const_block src) noexcept {
    auto [skip_len, data_len]{multi_byte::do_decode_length_and_code_point(src)};
    return std::make_tuple(ok{skip_len}, ok{data_len});
}
//------------------------------------------------------------------------------
// (content, data, padding)
export [[nodiscard]] auto get_padded_data_with_size(
  const memory::block src) noexcept
  -> std::tuple<memory::block, memory::block, memory::block> {
    const auto [skip_len, data_len]{get_skip_and_data_length(src)};
    if(skip_len and data_len) {
        return {
          skip(src, span_size(skip_len)),
          head(skip(src, span_size(skip_len)), span_size(data_len)),
          skip(src, span_size(skip_len) + span_size(data_len))};
    }
    return {{}, {}, {}};
}
//------------------------------------------------------------------------------
/// @brief Encodes the size of the source block into destination, copies data afterwards.
/// @ingroup serialization
/// @see get_data_with_size
export [[nodiscard]] auto store_data_with_size(
  const memory::const_block src,
  memory::block dst) noexcept -> memory::block {
    using multi_byte::code_point;

    if(const ok size_cp{limit_cast<code_point>(src.size())}) [[likely]] {
        using multi_byte::required_sequence_length;
        if(const ok size_len{required_sequence_length(size_cp)}) [[likely]] {
            if(size_len + src.size() <= dst.size()) {
                using multi_byte::encode_code_point;
                if(ok skip_len{encode_code_point(size_cp, dst)}) [[likely]] {
                    copy(src, skip(dst, span_size(skip_len)));
                    return head(dst, span_size(skip_len) + src.size());
                } else {
                    multi_byte::encode_nil(dst);
                }
            } else {
                multi_byte::encode_nil(dst);
            }
        }
    }

    return {};
}
//------------------------------------------------------------------------------
export [[nodiscard]] auto store_data_with_size(
  const memory::const_block src,
  memory::buffer& dst) noexcept -> memory::block {
    dst.resize(src.size() + 16);
    return store_data_with_size(src, cover(dst));
}
//------------------------------------------------------------------------------
/// @brief In a block starting with sub-block with size returns the size of the sub-block.
/// @ingroup serialization
/// @see store_data_with_size
/// @see get_data_with_size
export [[nodiscard]] auto skip_data_with_size(
  const memory::const_block src) noexcept -> span_size_t {
    const auto [skip_len, data_len]{get_skip_and_data_length(src)};
    if(skip_len and data_len) {
        return skip_len + data_len;
    }
    return 0;
}
//------------------------------------------------------------------------------
/// @brief Extracts a sub-block from a larger mutable block with encoded sub-block size.
/// @ingroup serialization
/// @see store_data_with_size
export [[nodiscard]] auto get_data_with_size(const memory::block src) noexcept
  -> memory::block {
    const memory::const_block tmp{src};
    const auto [skip_len, data_len]{get_skip_and_data_length(src)};
    if(skip_len and data_len) {
        return head(skip(src, span_size(skip_len)), span_size(data_len));
    }
    return {};
}
//------------------------------------------------------------------------------
/// @brief Extracts a sub-block from a larger const block with encoded sub-block size.
/// @ingroup serialization
/// @see store_data_with_size
export [[nodiscard]] auto get_data_with_size(
  const memory::const_block src) noexcept -> memory::const_block {
    const auto [skip_len, data_len]{get_skip_and_data_length(src)};
    if(skip_len and data_len) {
        return head(skip(src, span_size(skip_len)), span_size(data_len));
    }
    return {};
}
//------------------------------------------------------------------------------
/// @brief In a larger block with sub-blocks with size, calls function on each sub-block.
/// @ingroup serialization
/// @see store_data_with_size
/// @see get_data_with_size
export template <typename Function>
void for_each_data_with_size(
  memory::const_block src,
  Function function) noexcept {
    const auto f{[&](const span_size_t skip_len, const span_size_t data_len) {
        const auto po{src.begin()};
        const auto sz{src.size()};
        const auto sk{std::min(sz, skip_len + data_len)};

        function(memory::const_block{po + std::min(sz, skip_len), po + sk});
        src = memory::const_block{po + sk, po + sz};
    }};
    while(src) {
        const auto [skip_len, data_len]{get_skip_and_data_length(src)};
        if(data_len) {
            assert(skip_len);
            f(skip_len, data_len);
        } else {
            break;
        }
    }
}
//------------------------------------------------------------------------------
} // namespace eagine
