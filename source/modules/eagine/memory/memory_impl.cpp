/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module eagine.core.memory;

namespace eagine::memory {

auto default_shared_allocator() -> shared_byte_allocator {
    static const shared_byte_allocator alloc{default_byte_allocator()};
    return alloc;
}

} // namespace eagine::memory
