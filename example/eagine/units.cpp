/// @example eagine/units.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#if EAGINE_CORE_MODULE
import eagine.core;
import <iostream>;
#else
#include <eagine/console/console.hpp>
#include <eagine/main_ctx.hpp>
#include <eagine/tagged_quantity.hpp>
#include <eagine/units/dimensions.hpp>
#include <eagine/units/strings.hpp>
#include <eagine/units/unit/si.hpp>
#include <iostream>
#endif

namespace eagine {

template <typename Unit>
using qty = tagged_quantity<float, Unit>;

auto main(main_ctx& ctx) -> int {
    using namespace units;

    auto print = [&ctx](const auto& q) {
        using units::get_name;
        using units::get_name_form;
        using units::get_symbol;

        const auto dim = get_dimension(q.unit());
        ctx.cio()
          .print(
            identifier{"units"},
            "${dimName} (${dimForm}): "
            "${value} [${unitSymbol} (${unitForm})]")
          .arg(identifier{"dimName"}, get_name(dim))
          .arg(identifier{"dimForm"}, get_name_form(dim))
          .arg(identifier{"unitSymbol"}, get_symbol(q.unit()))
          .arg(identifier{"unitForm"}, get_name_form(q.unit()))
          .arg(identifier{"value"}, q.value());
    };

    qty<meter> x{2.F};
    qty<meter> y{3.F};
    qty<meter> z{4.F};
    qty<newton> f{12.F};
    qty<second> t{6.F};
    auto a = x * y;
    auto v = a * z;
    auto s = f / a;
    auto e = v * s;
    auto p = e / t;

    print(x);
    print(y);
    print(a);
    print(v);
    print(t);
    print(f);
    print(s);
    print(e);
    print(p);

    return 0;
}
} // namespace eagine

auto main(int argc, const char** argv) -> int {
    return eagine::default_main(argc, argv, eagine::main);
}

