/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.memory:data_baking;

import eagine.core.types;
import :block;
import :alloc_arena;
import :stack_allocator;

namespace eagine {
//------------------------------------------------------------------------------
export using data_bake_arena_base =
  memory::basic_allocation_arena<memory::stack_byte_allocator_only>;
//------------------------------------------------------------------------------
export class data_bake_arena : public data_bake_arena_base {
public:
    explicit data_bake_arena(const memory::block blk)
      : data_bake_arena_base(blk) {}

    auto baked_data() const noexcept -> memory::const_block {
        return this->allocator().allocated();
    }
};
//------------------------------------------------------------------------------
} // namespace eagine

