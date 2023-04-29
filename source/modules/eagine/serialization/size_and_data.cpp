/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.serialization:size_and_data;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.string;
import eagine.core.valid_if;

namespace eagine {
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
/// @brief In a block starting with sub-block with size returns the size of the sub-block.
/// @ingroup serialization
/// @see store_data_with_size
/// @see get_data_with_size
export [[nodiscard]] auto skip_data_with_size(
  const memory::const_block src) noexcept -> span_size_t {
    const ok skip_len{multi_byte::decode_sequence_length(src)};
    if(const ok data_len{multi_byte::do_decode_code_point(src, skip_len)}) {
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
    const ok skip_len{multi_byte::decode_sequence_length(tmp)};
    if(const ok data_len{multi_byte::do_decode_code_point(tmp, skip_len)}) {
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
    const ok skip_len{multi_byte::decode_sequence_length(src)};
    if(const ok data_len{multi_byte::do_decode_code_point(src, skip_len)}) {
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
    while(src) {
        if(const ok skip_len{multi_byte::decode_sequence_length(src)}) {
            using multi_byte::do_decode_code_point;
            if(const ok data_len{do_decode_code_point(src, skip_len)}) {
                function(
                  head(skip(src, span_size(skip_len)), span_size(data_len)));
                src = skip(src, span_size(skip_len) + span_size(data_len));
            } else {
                break;
            }
        } else {
            break;
        }
    }
}
//------------------------------------------------------------------------------
} // namespace eagine
