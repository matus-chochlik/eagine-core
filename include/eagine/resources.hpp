/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_RESOURCES_HPP
#define EAGINE_RESOURCES_HPP

#include "embed.hpp"
#include "logging/logger.hpp"
#include "main_ctx.hpp"
#include "memory/buffer_fwd.hpp"
#include "string_span.hpp"

namespace eagine {
//------------------------------------------------------------------------------
auto fetch_resource(
  const string_view description,
  const string_view key,
  const memory::const_block embedded_blk,
  memory::buffer& buf,
  application_config& args,
  const logger& log) -> memory::const_block;
//------------------------------------------------------------------------------
inline auto fetch_resource(
  const string_view description,
  const string_view key,
  const memory::const_block embedded_blk,
  main_ctx& ctx) -> memory::const_block {
    return fetch_resource(
      description,
      key,
      embedded_blk,
      ctx.scratch_space(),
      ctx.config(),
      ctx.log());
}
//------------------------------------------------------------------------------
} // namespace eagine

#if !EAGINE_CORE_LIBRARY || defined(EAGINE_IMPLEMENTING_LIBRARY)
#include <eagine/resources.inl>
#endif

#endif // EAGINE_RESOURCES_HPP
