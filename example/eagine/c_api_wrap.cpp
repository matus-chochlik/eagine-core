/// @example eagine/c_api_wrap.cpp
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
import eagine.core;
import std;

#if __has_include(<fcntl.h>) && \
	__has_include(<sys/types.h>) && \
	__has_include(<unistd.h>)
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#define EAGINE_POSIX 1
#define EXAMPLE_API_STATIC_FUNC(NAME) &::NAME
#else
#define EAGINE_POSIX 0
#define EXAMPLE_API_STATIC_FUNC(NAME) nullptr
#endif

namespace eagine {
//------------------------------------------------------------------------------
struct example_sets_errno {};
struct example_api_traits : c_api::default_traits {};
//------------------------------------------------------------------------------
struct example_file_api {
    example_api_traits _traits;

public:
    using this_api = example_file_api;
    using api_traits = example_api_traits;

    template <typename Result, typename... U>
    static constexpr auto check_result(Result result, U&&...) noexcept {
        return result;
    }

    c_api::combined_function<
      this_api,
      api_traits,
      example_sets_errno,
      char*(),
      string_view(),
      c_api::combined<c_api::c_string_view_map, c_api::trivial_map>,
      EXAMPLE_API_STATIC_FUNC(getlogin),
      EAGINE_POSIX,
      true>
      get_login;

    c_api::opt_function<
      api_traits,
      example_sets_errno,
      int(int[2]),
      EXAMPLE_API_STATIC_FUNC(pipe),
      EAGINE_POSIX,
      true>
      make_pipe;

    static auto _open(
      [[maybe_unused]] const char* path,
      [[maybe_unused]] int flags) -> int {
#if EAGINE_POSIX
        return ::open(path, flags); // NOLINT(hicpp-vararg)
#else
        return -1;
#endif
    }

    c_api::opt_function<
      api_traits,
      example_sets_errno,
      int(const char*, int),
      &_open,
      EAGINE_POSIX,
      true>
      open_file;

    c_api::opt_function<
      api_traits,
      example_sets_errno,
      ssize_t(int, void*, size_t),
      EXAMPLE_API_STATIC_FUNC(read),
      EAGINE_POSIX,
      true>
      read_file;

    c_api::simple_adapted_function<
      &example_file_api::read_file,
      c_api::head_transformed<ssize_t, 0, 2>(int, memory::block)>
      read_block{*this};

    c_api::opt_function<
      api_traits,
      example_sets_errno,
      ssize_t(int, const void*, size_t),
      EXAMPLE_API_STATIC_FUNC(write),
      EAGINE_POSIX,
      true>
      write_file;

    c_api::simple_adapted_function<
      &example_file_api::write_file,
      c_api::skip_transformed<ssize_t, 0, 2>(int, memory::const_block)>
      write_block{*this};

    c_api::simple_adapted_function<
      &example_file_api::write_file,
      c_api::skip_transformed<ssize_t, 0, 2>(int, string_view)>
      write_string{*this};

    c_api::opt_function<
      api_traits,
      example_sets_errno,
      int(int),
      EXAMPLE_API_STATIC_FUNC(close),
      EAGINE_POSIX,
      true>
      close_file;

    example_file_api()
      : get_login{"getlogin", *this}
      , make_pipe{"pipe", *this}
      , open_file{"open", *this}
      , read_file{"read", *this}
      , write_file{"write", *this}
      , close_file{"close", *this} {}

    auto traits() noexcept -> api_traits& {
        return _traits;
    }
};
//------------------------------------------------------------------------------
} // namespace eagine

auto main(int, const char** argv) -> int {
    using namespace eagine;
    using namespace eagine::c_api;

    example_file_api api;

    /*
    if(api.get_login) {
        std::cout << api.get_login() << std::endl;
    }
    */

    if(api.make_pipe and api.write_block and api.read_block and api.close_file) {
        int pfd[2] = {-1, -1};

        api.make_pipe(pfd);

        auto make_getbyte = [&api](int fd) {
            return [&api, fd]() -> std::optional<byte> {
                byte b{};
                if(not extract_or(api.read_block(fd, cover_one(b))).is_empty()) {
                    return {b};
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
