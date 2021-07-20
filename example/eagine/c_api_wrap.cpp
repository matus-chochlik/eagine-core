/// @example eagine/c_api_wrap.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
#include <eagine/config/platform.hpp>
#include <eagine/c_api_wrap.hpp>
#include <eagine/extract.hpp>
#include <eagine/hexdump.hpp>
#include <eagine/maybe_unused.hpp>
#include <eagine/memory/block.hpp>
#include <iostream>

#if EAGINE_POSIX
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#define EXAMPLE_API_STATIC_FUNC(NAME) &::NAME
#else
#define EXAMPLE_API_STATIC_FUNC(NAME) nullptr
#endif

namespace eagine {
//------------------------------------------------------------------------------
struct example_sets_errno {};
//------------------------------------------------------------------------------
struct example_api_traits : default_c_api_traits {};
//------------------------------------------------------------------------------
struct example_file_api {

    using this_api = example_file_api;
    using api_traits = example_api_traits;

    template <typename Tag = example_sets_errno>
    using derived_func = derived_c_api_function<this_api, api_traits, Tag>;

    opt_c_api_function<
      api_traits,
      example_sets_errno,
      int(int[2]),
      EXAMPLE_API_STATIC_FUNC(pipe),
      EAGINE_POSIX,
      true>
      make_pipe;

    struct : derived_func<> {
        using base = derived_func<>;
        using base::base;

        explicit constexpr operator bool() const noexcept {
            return EAGINE_POSIX != 0;
        }

        auto operator()(const char* path, int flags) noexcept -> int {
            EAGINE_MAYBE_UNUSED(path);
            EAGINE_MAYBE_UNUSED(flags);
#if EAGINE_POSIX
            return ::open(path, flags); // NOLINT(hicpp-vararg)
#else
            return -1;
#endif
        }
    } open_file;

    opt_c_api_function<
      api_traits,
      example_sets_errno,
      ssize_t(int, void*, size_t),
      EXAMPLE_API_STATIC_FUNC(read),
      EAGINE_POSIX,
      true>
      read_file;

    struct : derived_func<> {
        using base = derived_func<>;
        using base::api;
        using base::base;

        explicit constexpr operator bool() const noexcept {
            return bool(api().read_file);
        }

        auto operator()(int fd, memory::block blk) noexcept -> ssize_t {
            return api().read_file(
              fd,
              static_cast<void*>(blk.data()),
              static_cast<size_t>(blk.size()));
        }
    } read_block;

    opt_c_api_function<
      api_traits,
      example_sets_errno,
      ssize_t(int, const void*, size_t),
      EXAMPLE_API_STATIC_FUNC(write),
      EAGINE_POSIX,
      true>
      write_file;

    struct : derived_func<> {
        using base = derived_func<>;
        using base::api;
        using base::base;

        explicit constexpr operator bool() const noexcept {
            return bool(api().write_file);
        }

        auto operator()(int fd, memory::const_block blk) noexcept -> ssize_t {
            return api().write_file(
              fd,
              static_cast<const void*>(blk.data()),
              static_cast<size_t>(blk.size()));
        }
    } write_block;

    struct : derived_func<> {
        using base = derived_func<>;
        using base::api;
        using base::base;

        explicit constexpr operator bool() const noexcept {
            return bool(api().write_file);
        }

        auto operator()(int fd, string_view str) noexcept -> ssize_t {
            return api().write_file(
              fd,
              static_cast<const void*>(str.data()),
              static_cast<size_t>(str.size()));
        }
    } write_string;

    opt_c_api_function<
      api_traits,
      example_sets_errno,
      int(int),
      EXAMPLE_API_STATIC_FUNC(close),
      EAGINE_POSIX,
      true>
      close_file;

    example_file_api(api_traits& traits)
      : make_pipe{"pipe", traits, *this}
      , open_file{"open", traits, *this}
      , read_file{"read", traits, *this}
      , read_block("read", traits, *this)
      , write_file{"write", traits, *this}
      , write_block("write", traits, *this)
      , write_string("write", traits, *this)
      , close_file{"close", traits, *this} {}
};
//------------------------------------------------------------------------------
} // namespace eagine

auto main(int, const char** argv) -> int {
    using namespace eagine;
    example_api_traits traits;
    example_file_api api(traits);

    if(api.make_pipe && api.write_block && api.read_block && api.close_file) {
        int pfd[2] = {-1, -1};

        api.make_pipe(pfd);

        auto make_getbyte = [&api](int fd) {
            return [&api, fd]() -> optionally_valid<byte> {
                byte b{};
                if(api.read_block(fd, cover_one(b)) > 0) {
                    return {b, true};
                }
                api.close_file(fd);
                return {};
            };
        };

        auto putchar = [](char c) {
            std::cout << c;
            return true;
        };

        if(api.open_file) {
            auto fd = api.open_file(argv[0], 0);
            if(fd >= 0) {
                auto getbyte = make_getbyte(fd);
                while(auto optb = getbyte()) {
                    api.write_block(pfd[1], view_one(extract(optb)));
                }
            }
        }

        api.write_string(pfd[1], "some test string");
        api.write_string(pfd[1], "one other string");
        api.close_file(pfd[1]);

        hexdump::apply(
          {construct_from, make_getbyte(pfd[0])}, {construct_from, putchar});
    } else {
        std::cerr << "required API is not available" << std::endl;
    }

    return 0;
}
