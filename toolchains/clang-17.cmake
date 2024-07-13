# Copyright Matus Chochlik.
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt
#
# Copy this file to `toolchain.cmake` and change as necessary.
# Update if the llvm install prefix path is different
#set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
#set(CMAKE_CXX_COMPILER_LAUNCHER ccache)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_COMPILER clang-17)
set(CMAKE_CXX_COMPILER clang++-17)
set(LLVM_PROFDATA llvm-profdata-17)
set(CMAKE_CXX_FLAGS -stdlib=libc++)
set(CMAKE_EXE_LINKER_FLAGS -stdlib=libc++)
set(CMAKE_EXE_LINKER_FLAGS -fuse-ld=lld-17)
set(CMAKE_SHARED_LINKER_FLAGS -fuse-ld=lld-17)

