/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_C_API_PARAMETER_MAP_HPP
#define EAGINE_C_API_PARAMETER_MAP_HPP

#include "../int_constant.hpp"
#include "../string_span.hpp"

namespace eagine::c_api {
//------------------------------------------------------------------------------
class trivial_map {

    template <typename U, typename P1, typename... Params>
    constexpr auto _get(size_constant<1>, U, P1 p1, Params...) const noexcept {
        return p1;
    }

    template <std::size_t I, typename U, typename Pi, typename... Params>
    constexpr auto _get(size_constant<I>, U u, Pi, Params... params)
      const noexcept {
        return _get(size_constant<I - 1>(), u, params...);
    }

public:
    template <typename RV, typename... Params>
    constexpr auto operator()(size_constant<0>, RV rv, Params...)
      const noexcept {
        return rv;
    }

    template <std::size_t I, typename... Params>
    constexpr auto operator()(size_constant<I> i, Params... params)
      const noexcept {
        return _get(i, params...);
    }
};
//------------------------------------------------------------------------------
template <std::size_t... J>
struct trivial_arg_map {
    template <std::size_t I, typename... Params>
    constexpr auto operator()(size_constant<I> i, Params... params)
      const noexcept requires(... || (I == J)) {
        return trivial_map{}(i, params...);
    }
};
//------------------------------------------------------------------------------
template <std::size_t CI, std::size_t CppI>
struct get_data_map {
    template <typename... Params>
    constexpr auto operator()(size_constant<CI>, Params... params)
      const noexcept {
        return trivial_map{}(size_constant<CppI>{}, params...).data();
    }
};

template <std::size_t CI, std::size_t CppI>
struct get_size_map {
    template <typename... Params>
    constexpr auto operator()(size_constant<CI>, Params... params)
      const noexcept {
        return trivial_map{}(size_constant<CppI>{}, params...).size();
    }
};

template <std::size_t CDI, std::size_t CSI, std::size_t CppI>
struct get_data_size_map {
    template <typename... Params>
    constexpr auto operator()(size_constant<CDI>, Params... params)
      const noexcept {
        return trivial_map{}(size_constant<CppI>{}, params...).data();
    }

    template <typename... Params>
    constexpr auto operator()(size_constant<CSI>, Params... params)
      const noexcept {
        return trivial_map{}(size_constant<CppI>{}, params...).size();
    }
};

struct c_string_view_map {
    template <typename... Params>
    constexpr auto operator()(size_constant<0> i, Params... params)
      const noexcept {
        const trivial_map map;
        return map(i, params...)
          .transformed([=](const char* cstr, bool is_valid) -> string_view {
              if(is_valid && cstr) {
                  return string_view{cstr};
              }
              return {};
          });
    }
};

template <std::size_t CppI>
struct head_transform_map {
    template <typename... Params>
    constexpr auto operator()(size_constant<0> i, Params... params)
      const noexcept {
        const trivial_map map;
        return map(i, params...).transformed([=](auto len, bool is_valid) {
            auto res{map(size_constant<CppI>{}, params...)};
            if(is_valid) {
                return head(res, len);
            }
            return head(res, 0);
        });
    }
};

template <std::size_t CppI>
struct skip_transform_map {
    template <typename... Params>
    constexpr auto operator()(size_constant<0> i, Params... params)
      const noexcept {
        const trivial_map map;
        return map(i, params...).transformed([=](auto len, bool is_valid) {
            auto res{map(size_constant<CppI>{}, params...)};
            if(is_valid) {
                return skip(res, len);
            }
            return skip(res, 0);
        });
    }
};
//------------------------------------------------------------------------------
template <typename T, typename M>
struct convert;

template <typename T, std::size_t... J>
struct convert<T, trivial_arg_map<J...>> {
    template <std::size_t I, typename... Params>
    constexpr auto operator()(size_constant<I> i, Params... params)
      const noexcept requires(... || (I == J)) {
        return static_cast<T>(trivial_map{}(i, params...));
    }
};

template <typename T, std::size_t CI, std::size_t CppI>
struct convert<T, get_data_map<CI, CppI>> {
    template <typename... Params>
    constexpr auto operator()(size_constant<CI> i, Params... params)
      const noexcept {
        return static_cast<T>(get_data_map<CI, CppI>{}(i, params...));
    }
};

template <typename T, std::size_t CI, std::size_t CppI>
struct convert<T, get_size_map<CI, CppI>> {
    template <typename... Params>
    constexpr auto operator()(size_constant<CI> i, Params... params)
      const noexcept {
        return static_cast<T>(get_size_map<CI, CppI>{}(i, params...));
    }
};
//------------------------------------------------------------------------------
template <typename... M>
struct combined_map : M... {
    using M::operator()...;
};
//------------------------------------------------------------------------------
} // namespace eagine::c_api

#endif

