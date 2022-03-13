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

namespace eagine::c_api {
//------------------------------------------------------------------------------
class trivial_map {

    template <typename P1, typename... Params>
    constexpr auto _get(size_constant<1>, P1 p1, Params...) const noexcept {
        return p1;
    }

    template <std::size_t I, typename P1, typename... Params>
    constexpr auto _get(size_constant<I>, P1, Params... params) const noexcept {
        return _get(size_constant<I - 1>(), params...);
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
template <std::size_t I>
struct trivial_arg_map {
    template <typename... Params>
    constexpr auto operator()(size_constant<I> i, Params... params)
      const noexcept {
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
//------------------------------------------------------------------------------
template <std::size_t I, typename T>
struct convert_arg_map {
    template <typename... Params>
    constexpr auto operator()(size_constant<I>, Params... params)
      const noexcept {
        constexpr trivial_map map;
        return T(map(size_constant<I>{}, params...));
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

