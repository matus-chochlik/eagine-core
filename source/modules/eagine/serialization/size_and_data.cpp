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

    const auto opt_size_cp = limit_cast<multi_byte::code_point>(src.size());
    if(opt_size_cp) [[likely]] {
        const auto size_cp = extract(opt_size_cp);
        const auto opt_size_len = multi_byte::required_sequence_length(size_cp);
        if(opt_size_len) [[likely]] {
            if(extract(opt_size_len) + src.size() <= dst.size()) {
                const auto opt_skip_len =
                  multi_byte::encode_code_point(size_cp, dst);
                if(opt_skip_len) [[likely]] {
                    const auto skip_len =
                      limit_cast<span_size_t>(extract(opt_skip_len));
                    copy(src, skip(dst, skip_len));
                    return head(dst, skip_len + src.size());
                }
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
    const auto opt_skip_len = multi_byte::decode_sequence_length(src);
    if(const auto opt_data_len{
         multi_byte::do_decode_code_point(src, opt_skip_len)}) {
        return extract(opt_skip_len) + extract(opt_data_len);
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
    const auto opt_skip_len = multi_byte::decode_sequence_length(tmp);
    if(const auto opt_data_len{
         multi_byte::do_decode_code_point(tmp, opt_skip_len)}) {
        return head(
          skip(src, extract(opt_skip_len)),
          limit_cast<span_size_t>(extract(opt_data_len)));
    }
    return {};
}
//------------------------------------------------------------------------------
/// @brief Extracts a sub-block from a larger const block with encoded sub-block size.
/// @ingroup serialization
/// @see store_data_with_size
export [[nodiscard]] auto get_data_with_size(
  const memory::const_block src) noexcept -> memory::const_block {
    const auto opt_skip_len = multi_byte::decode_sequence_length(src);
    if(const auto opt_data_len{
         multi_byte::do_decode_code_point(src, opt_skip_len)}) {
        return head(
          skip(src, extract(opt_skip_len)),
          limit_cast<span_size_t>(extract(opt_data_len)));
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
        const auto opt_skip_len = multi_byte::decode_sequence_length(src);
        const auto opt_data_len =
          multi_byte::do_decode_code_point(src, opt_skip_len);
        if(opt_data_len) {
            const auto skip_len = extract(opt_skip_len);
            const auto data_len = extract(opt_data_len);
            if(data_len > 0) {
                function(
                  head(skip(src, skip_len), limit_cast<span_size_t>(data_len)));
                src = skip(src, skip_len + data_len);
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
