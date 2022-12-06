set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_COMPILER clang-16)
set(CMAKE_CXX_COMPILER clang++-16)
set(CMAKE_CXX_FLAGS -stdlib=libc++)
set(LLVM_PROFDATA llvm-profdata-16)
set(CMAKE_EXE_LINKER_FLAGS "-stdlib=libc++ -fuse-ld=lld")
set(CMAKE_SHARED_LINKER_FLAGS "-fuse-ld=lld")

