/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_MEMORY_FWD_HPP
#define EAGINE_MEMORY_FWD_HPP

#include "../nothing.hpp"

namespace eagine::memory {

class buffer;

template <typename Base>
class basic_shared_byte_alloc;
using shared_byte_allocator = basic_shared_byte_alloc<nothing_t>;

} // namespace eagine::memory

#endif
