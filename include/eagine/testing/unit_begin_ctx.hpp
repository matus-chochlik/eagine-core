/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt
///
///
#include "unit_begin.hpp"
import eagine.core.main_ctx;

namespace eagitest {
//------------------------------------------------------------------------------
class ctx_suite : public suite {
public:
    ctx_suite(
      eagine::test_ctx& ctx,
      std::string_view name,
      suite_case_t cases) noexcept
      : suite{ctx.args().argc(), ctx.args().argv(), name, cases}
      , _ctx{ctx} {}

    [[nodiscard]] auto ctx() const noexcept -> eagine::test_ctx& {
        return _ctx;
    }

private:
    eagine::test_ctx& _ctx;
};
//------------------------------------------------------------------------------
} // namespace eagitest
