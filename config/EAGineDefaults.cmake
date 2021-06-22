#  Copyright Matus Chochlik.
#  Distributed under the Boost Software License, Version 1.0.
#  See accompanying file LICENSE_1_0.txt or copy at
#   http://www.boost.org/LICENSE_1_0.txt
#

option(CONFIG_INFO_ONLY "Only generate the config/info.py script" Off)
option(CONFIG_SET_LOW_PROFILE "Set the LOW_PROFILE switch in site_config.hpp." Off)

option(NO_EXAMPLES "Don't configure examples" Off)
option(NO_TESTS "Don't configure tests." Off)

option(WITH_CLANG_TIDY "Configure with clang-tidy checks." Off)
