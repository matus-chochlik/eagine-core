/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

module eagine.core.app;

import eagine.core.build_info;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.identifier;
import eagine.core.main_ctx;
import eagine.core.resource;
import std;

namespace eagine {
//------------------------------------------------------------------------------
// version
//------------------------------------------------------------------------------
void print_version(main_ctx&, std::ostream& out) {
    out << config_git_version().value_or("unknown") << std::endl;
}
//------------------------------------------------------------------------------
// copyright / license
//------------------------------------------------------------------------------
void print_copyright_notice(main_ctx& ctx, std::ostream& out) {
    const auto print{[&](const memory::const_block data) {
        write_to_stream(out, data);
        return true;
    }};

    out << "Copyright © 2015-2024 Matúš Chochlík.\n";

    if(const auto license_res{search_resource("LicenseTxt")}) {
        out << "Distributed under the following license:\n";
        out << '\n';
        license_res.fetch(ctx, {construct_from, print});
    } else if(const auto boost_license_res{search_resource("BoostLcnse")}) {
        out << "Distributed under the Boost Software License, Version 1.0.\n";
        out << '\n';
        boost_license_res.fetch(ctx, {construct_from, print});
    } else {
        out << "Distributed under the Boost Software License, Version 1.0.\n";
        out << "See accompanying file LICENSE_1_0.txt or copy at\n";
        out << " http://www.boost.org/LICENSE_1_0.txt\n";
    }
}
//------------------------------------------------------------------------------
// bash completion from resource
//------------------------------------------------------------------------------
void do_print_bash_completion(
  main_ctx& ctx,
  std::ostream& out,
  const embedded_resource& res) {
    if(res) {
        const auto print{[&](const memory::const_block data) {
            write_to_stream(out, data);
            return true;
        }};
        res.fetch(ctx, {construct_from, print});
    }
}
//------------------------------------------------------------------------------
void print_bash_completion(main_ctx& ctx, std::ostream& out) {
    print_bash_completion(ctx, out, "BashCmpltn");
}
//------------------------------------------------------------------------------
// common special arguments
//------------------------------------------------------------------------------
auto handle_common_special_args(main_ctx& ctx) -> bool {
    if(ctx.args().find("--version")) {
        print_version(ctx, std::cout);
        return true;
    }
    if(ctx.args().find("--copyright") or ctx.args().find("--license")) {
        print_copyright_notice(ctx, std::cout);
        return true;
    }
    if(ctx.args().find("--print-bash-completion")) {
        print_bash_completion(ctx, std::cout);
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------
} // namespace eagine
