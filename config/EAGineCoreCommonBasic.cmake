#  Copyright Matus Chochlik.
#  Distributed under the Boost Software License, Version 1.0.
#  See accompanying file LICENSE_1_0.txt or copy at
#  https://www.boost.org/LICENSE_1_0.txt
#
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

include(EAGinePolicies)
include(EAGineDefaults)
include(EAGineBuildType)
include(EAGineArchitecture)
include(EAGineStaticAnalysis)
include(EAGineCompiler)
include(EAGineLowProfile)
include(EAGineCommonFindMod)
include(EAGineCMake)
include(EAGineGitInfo)
include(EAGinePackage)
