/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include "interfaces.hpp"

namespace eagine::logs {
//------------------------------------------------------------------------------
auto make_sink_factory(main_ctx& ctx) noexcept
  -> shared_holder<stream_sink_factory> {
    return make_ostream_sink_factory(ctx);
}
//------------------------------------------------------------------------------
auto make_data_parser(main_ctx& ctx, shared_holder<stream_sink> sink) noexcept
  -> valtree::value_tree_stream_input {
    return make_json_parser(ctx, std::move(sink));
}
//------------------------------------------------------------------------------
auto make_reader(
  main_ctx& ctx,
  shared_holder<stream_sink_factory> factory) noexcept
  -> unique_holder<reader> {
    return make_cin_reader(ctx, std::move(factory));
}
//------------------------------------------------------------------------------
} // namespace eagine::logs
