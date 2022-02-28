/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include <eagine/branch_predict.hpp>
#include <eagine/extract.hpp>
#include <eagine/main_ctx_fwd.hpp>
#include <eagine/memory/default_alloc.hpp>

namespace eagine::memory {

EAGINE_LIB_FUNC
auto default_shared_allocator() -> shared_byte_allocator {
    const auto ctx{try_get_main_ctx()};
    if(EAGINE_LIKELY(ctx)) {
        return extract(ctx).default_allocator();
    }
    return shared_byte_allocator{default_byte_allocator()};
}

} // namespace eagine::memory
