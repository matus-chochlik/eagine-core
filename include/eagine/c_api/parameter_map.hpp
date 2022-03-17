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
#include "../mp_list.hpp"
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

template <typename T>
struct cast_to_map {
    template <typename... Params>
    constexpr auto operator()(size_constant<0> i, Params... params)
      const noexcept {
        return trivial_map{}(i, params...).cast_to(type_identity<T>{});
    }
};

struct c_string_view_map {
    template <typename... Params>
    constexpr auto operator()(size_constant<0> i, Params... params)
      const noexcept {
        return trivial_map{}(i, params...)
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
template <std::size_t I, typename CP, typename CppP>
struct make_arg_map : convert<CP, trivial_arg_map<I>> {};

template <std::size_t I, typename P>
struct make_arg_map<I, P, P> : trivial_arg_map<I> {};
//------------------------------------------------------------------------------
template <std::size_t I, typename CP, typename CppP>
struct make_args_map;

template <std::size_t I>
struct make_args_map<I, mp_list<>, mp_list<>> {
protected:
    void operator()() const noexcept {};
};

template <std::size_t I, typename CH, typename... CT, typename CppH, typename... CppT>
struct make_args_map<I, mp_list<CH, CT...>, mp_list<CppH, CppT...>>
  : make_arg_map<I, CH, CppH>
  , make_args_map<I + 1, mp_list<CT...>, mp_list<CppT...>> {
    using make_arg_map<I, CH, CppH>::operator();
    using make_args_map<I + 1, mp_list<CT...>, mp_list<CppT...>>::operator();
};
//------------------------------------------------------------------------------
template <typename CSignature, typename CppSignature>
static auto make_map(CSignature*, CppSignature*) -> trivial_map;

template <typename CRV, typename CppRV>
static auto make_map(CRV (*)(), CppRV (*)())
  -> cast_to_map<CppRV> requires(!std::is_same_v<CRV, CppRV>);

template <typename RV, typename... CParam, typename... CppParam>
static auto make_map(RV (*)(CParam...), RV (*)(CppParam...))
  -> make_args_map<0, mp_list<RV, CParam...>, mp_list<RV, CppParam...>> requires(
    (sizeof...(CParam) > 0) && (sizeof...(CParam) == sizeof...(CppParam)));

template <typename CRV, typename CppRV, typename... CParam, typename... CppParam>
static auto make_map(CRV (*)(CParam...), CppRV (*)(CppParam...))
  -> combined_map<cast_to_map<CppRV>, make_args_map<1, mp_list<CParam...>, mp_list<CppParam...>>> requires(
    !std::is_same_v<CRV, CppRV> && (sizeof...(CParam) > 0) &&
    (sizeof...(CParam) == sizeof...(CppParam)));

template <typename CSignature, typename CppSignature>
using make_map_t = decltype(make_map(
  static_cast<CSignature*>(nullptr),
  static_cast<CppSignature*>(nullptr)));
//------------------------------------------------------------------------------
} // namespace eagine::c_api

#endif

