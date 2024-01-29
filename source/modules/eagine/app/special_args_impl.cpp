/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
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
void print_version(main_ctx&) {
    std::cout << config_git_version().value_or("unknown") << std::endl;
}
//------------------------------------------------------------------------------
void print_git_hash(main_ctx&) {
    std::cout << config_git_hash_id().value_or("unknown") << std::endl;
}
//------------------------------------------------------------------------------
// copyright / license
//------------------------------------------------------------------------------
void print_copyright_notice(main_ctx& ctx) {
    using std::cout;

    const auto print{[&](const memory::const_block data) {
        write_to_stream(cout, data);
        return true;
    }};

    cout << "Copyright © 2015-2024 Matúš Chochlík.\n";

    if(const auto license_res{search_resource("LicenseTxt")}) {
        cout << "Distributed under the following license:\n";
        cout << '\n';
        license_res.fetch(ctx, {construct_from, print});
    } else if(const auto boost_license_res{search_resource("BoostLcnse")}) {
        cout << "Distributed under the Boost Software License, Version 1.0.\n";
        cout << '\n';
        boost_license_res.fetch(ctx, {construct_from, print});
    } else {
        cout << "Distributed under the Boost Software License, Version 1.0.\n";
        cout << "See accompanying file LICENSE_1_0.txt or copy at\n";
        cout << " https://www.boost.org/LICENSE_1_0.txt\n";
    }
}
//------------------------------------------------------------------------------
// print text from resource
//------------------------------------------------------------------------------
void print_resource(main_ctx& ctx, const embedded_resource& res) {
    if(res) {
        const auto print{[&](const memory::const_block data) {
            write_to_stream(std::cout, data);
            return true;
        }};
        res.fetch(ctx, {construct_from, print});
    }
}
//------------------------------------------------------------------------------
// common special arguments
//------------------------------------------------------------------------------
auto handle_common_special_args(main_ctx& ctx) -> std::optional<int> {
    if(ctx.args().find("--eagine-version") or ctx.args().find("--version")) {
        print_version(ctx);
        return {0};
    }
    if(ctx.args().find("--eagine-git-hash") or ctx.args().find("--git-hash")) {
        print_git_hash(ctx);
        return {0};
    }
    if(const auto arg{ctx.args().find("--version-at-least")}) {
        if(const auto result{ctx.version().version_at_least(arg.next())}) {
            return {0};
        } else if(not result) {
            return {1};
        }
        return {2};
    }
    if(
      ctx.args().find("--eagine-license") or ctx.args().find("--license") or
      ctx.args().find("--copyright")) {
        print_copyright_notice(ctx);
        return {0};
    }
    if(ctx.args().find("--print-bash-completion")) {
        if(const auto res{search_resource("BashCmpltn")}) {
            print_resource(ctx, res);
        }
        return {0};
    }
    if(ctx.args().find("--print-manual")) {
        if(const auto res{search_resource("Manual")}) {
            print_resource(ctx, res);
        }
        return {0};
    }
    return {};
}
//------------------------------------------------------------------------------
} // namespace eagine
