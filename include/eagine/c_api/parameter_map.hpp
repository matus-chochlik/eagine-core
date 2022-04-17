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
#include "../is_within_limits.hpp"
#include "../memory/split_span.hpp"
#include "../mp_list.hpp"
#include "../string_span.hpp"
#include "buffer_data.hpp"
#include "handle.hpp"
#include "key_value_list.hpp"

namespace eagine::c_api {
//------------------------------------------------------------------------------
class trivial_map {

    template <typename U, typename P1, typename... P>
    constexpr auto _get(size_constant<1>, U, P1& p1, P&...) const noexcept
      -> decltype(auto) {
        return p1;
    }

    template <std::size_t I, typename U, typename Pi, typename... P>
    constexpr auto _get(size_constant<I>, U u, Pi&, P&... p) const noexcept
      -> decltype(auto) {
        return _get(size_constant<I - 1>(), u, p...);
    }

public:
    template <typename A, typename RV, typename... P>
    constexpr auto operator()(size_constant<0>, const A&, RV rv, P&...)
      const noexcept -> decltype(auto) {
        return rv;
    }

    template <std::size_t I, typename A, typename... P>
    constexpr auto operator()(size_constant<I> i, const A&, P&&... p)
      const noexcept -> decltype(auto) {
        return _get(i, std::forward<P>(p)...);
    }
};

template <std::size_t... J>
struct trivial_arg_map {
    template <std::size_t I, typename... P>
    constexpr auto operator()(size_constant<I> i, P&&... p) const noexcept
      -> decltype(auto) requires(... || (I == J)) {
        return trivial_map{}(i, std::forward<P>(p)...);
    }
};
//------------------------------------------------------------------------------
template <std::size_t... J>
struct nullptr_arg_map {
    template <std::size_t I, typename... P>
    constexpr auto operator()(size_constant<I>, P&&...) const noexcept
      -> decltype(auto) requires(... || (I == J)) {
        return nullptr;
    }
};
//------------------------------------------------------------------------------
template <std::size_t CI, std::size_t CppI>
struct reorder_arg_map {
    template <typename... P>
    constexpr auto operator()(size_constant<CI>, P&&... p) const noexcept
      -> decltype(auto) {
        return trivial_map{}(size_constant<CppI>{}, std::forward<P>(p)...);
    }
};
//------------------------------------------------------------------------------
template <std::size_t CI, typename T>
struct get_ptr_arg_map {
    template <typename... P>
    constexpr auto operator()(size_constant<0> i, P&&... p) const noexcept {
        return trivial_map{}(i, std::forward<P>(p)...).replaced_with(_value);
    }

    template <typename... P>
    constexpr auto operator()(size_constant<CI>, P&&...) noexcept -> T* {
        return &_value;
    }

private:
    T _value{};
};
//------------------------------------------------------------------------------
template <std::size_t CI, std::size_t CppI>
struct addressof_map {
    template <typename... P>
    constexpr auto operator()(size_constant<CI> i, P&&... p) const noexcept {
        return std::addressof(
          reorder_arg_map<CI, CppI>{}(i, std::forward<P>(p)...));
    }
};

template <std::size_t CI, std::size_t CppI>
struct get_data_map {
    template <typename... P>
    constexpr auto operator()(size_constant<CI> i, P&&... p) const noexcept {
        return reorder_arg_map<CI, CppI>{}(i, std::forward<P>(p)...).data();
    }
};

template <std::size_t CI, std::size_t CppI>
struct get_size_map {
    template <typename... P>
    constexpr auto operator()(size_constant<CI> i, P&&... p) const noexcept {
        return reorder_arg_map<CI, CppI>{}(i, std::forward<P>(p)...).size();
    }
};

template <std::size_t CI, std::size_t CppI, auto chunkSize>
struct get_chunk_size_map {
    template <typename... P>
    constexpr auto operator()(size_constant<CI> i, P&&... p) const noexcept {
        return reorder_arg_map<CI, CppI>{}(i, std::forward<P>(p)...).size() /
               chunkSize;
    }
};

template <std::size_t CDI, std::size_t CSI, std::size_t CppI>
struct get_data_size_map {
    template <typename... P>
    constexpr auto operator()(size_constant<CDI> i, P&&... p) const noexcept {
        return reorder_arg_map<CDI, CppI>{}(i, std::forward<P>(p)...).data();
    }

    template <typename... P>
    constexpr auto operator()(size_constant<CSI> i, P&&... p) const noexcept {
        return reorder_arg_map<CSI, CppI>{}(i, std::forward<P>(p)...).size();
    }
};

template <typename CP, typename CppP>
struct cast_to_map {
    template <typename... P>
    constexpr auto operator()(size_constant<0> i, P&&... p) const noexcept {
        if constexpr(
          std::is_convertible_v<CP, CppP> ||
          std::is_constructible_v<CppP, CP>) {
            return trivial_map{}(i, std::forward<P>(p)...)
              .cast_to(type_identity<CppP>{});
        } else if constexpr(std::is_default_constructible_v<CppP>) {
            return trivial_map{}(i, std::forward<P>(p)...).replaced_with(CppP{});
        }
    }
};

struct collapse_map {
    template <typename A, typename... P>
    constexpr auto operator()(size_constant<0> i, A& api, P&&... p)
      const noexcept {
        return api.collapse(
          c_api::trivial_map{}(i, api, std::forward<P>(p)...));
    }
};

template <std::size_t CppI>
struct replaced_with_map {
    template <typename... P>
    constexpr auto operator()(size_constant<0> i, P&&... p) const noexcept {
        return trivial_map{}(i, std::forward<P>(p)...)
          .replaced_with(
            trivial_map{}(size_constant<CppI>{}, std::forward<P>(p)...));
    }
};

struct c_string_view_map {
    template <typename... P>
    constexpr auto operator()(size_constant<0> i, P&&... p) const noexcept {
        return trivial_map{}(i, std::forward<P>(p)...)
          .transformed([=](const char* cstr, bool is_valid) -> string_view {
              if(is_valid && cstr) {
                  return string_view{cstr};
              }
              return {};
          });
    }
};

template <typename S, std::size_t CppSpanI, std::size_t CSizeI = 0>
struct head_transform_map {
    template <typename... P>
    constexpr auto operator()(size_constant<0> i, P... p) const noexcept {
        const trivial_map map;
        return map(i, p...).transformed([=, this](auto, bool is_valid) {
            auto res{map(size_constant<CppSpanI>{}, p...)};
            if(is_valid) {
                return head(res, _len);
            }
            return head(res, 0);
        });
    }

    template <typename... P>
    constexpr auto operator()(size_constant<CSizeI> i, P... p) noexcept -> S* {
        _len = limit_cast<S>(get_size_map<CSizeI, CppSpanI>{}(i, p...));
        return &_len;
    }

private:
    S _len{};
};

template <typename S, std::size_t CppSpanI>
struct head_transform_map<S, CppSpanI, 0> {
    template <typename... P>
    constexpr auto operator()(size_constant<0> i, P... p) const noexcept {
        const trivial_map map;
        return map(i, p...).transformed([=](auto len, bool is_valid) {
            auto res{map(size_constant<CppSpanI>{}, p...)};
            if(is_valid) {
                return head(res, len);
            }
            return head(res, 0);
        });
    }
};

template <typename S, std::size_t CppSpanI, std::size_t CSizeI = 0>
struct skip_transform_map {
    template <typename... P>
    constexpr auto operator()(size_constant<0> i, P... p) const noexcept {
        const trivial_map map;
        return map(i, p...).transformed([=, this](auto, bool is_valid) {
            auto res{map(size_constant<CppSpanI>{}, p...)};
            if(is_valid) {
                return skip(res, _len);
            }
            return skip(res, 0);
        });
    }

    template <typename... P>
    constexpr auto operator()(size_constant<CSizeI> i, P... p) noexcept -> S* {
        _len = limit_cast<S>(get_size_map<CSizeI, CppSpanI>{}(i, p...));
        return &_len;
    }

private:
    S _len{};
};

template <typename S, std::size_t CppSpanI>
struct skip_transform_map<S, CppSpanI, 0> {
    template <typename... P>
    constexpr auto operator()(size_constant<0> i, P... p) const noexcept {
        const trivial_map map;
        return map(i, p...).transformed([=](auto len, bool is_valid) {
            auto res{map(size_constant<CppSpanI>{}, p...)};
            if(is_valid) {
                return skip(res, len);
            }
            return skip(res, 0);
        });
    }
};

template <typename S, std::size_t CppSpanI, std::size_t CSizeI = 0>
struct split_transform_map {
    template <typename... P>
    constexpr auto operator()(size_constant<0> i, P... p) const noexcept {
        const trivial_map map;
        return map(i, p...).replaced_with(
          map(size_constant<CppSpanI>{}, p...).advance(span_size(_len)));
    }

    template <typename... P>
    constexpr auto operator()(size_constant<CSizeI>, P... p) noexcept -> S* {
        const trivial_map map;
        _len = limit_cast<S>(map(size_constant<CppSpanI>{}, p...).tail.size());
        return &_len;
    }

private:
    S _len{};
};

template <typename S, std::size_t CppSpanI>
struct split_transform_map<S, CppSpanI, 0> {
    template <typename... P>
    constexpr auto operator()(size_constant<0> i, P... p) const noexcept {
        const trivial_map map;
        return map(i, p...).transformed([=](auto len, bool is_valid) {
            auto res{map(size_constant<CppSpanI>{}, p...)};
            if(is_valid) {
                return res.advance(span_size(len));
            }
            return res;
        });
    }
};
//------------------------------------------------------------------------------
template <typename Dst, typename Src>
static constexpr auto c_arg_cast(Src&& src) noexcept -> Dst {
    if constexpr((std::is_integral_v<Dst> || std::is_floating_point_v<Dst>)&&(
                   std::is_integral_v<Src> || std::is_floating_point_v<Src>)) {
        if constexpr(std::is_same_v<
                       std::remove_cv_t<std::remove_reference_t<Src>>,
                       bool>) {
            return std::forward<Src>(src) ? 1 : 0;
        } else {
            return limit_cast<Dst>(std::forward<Src>(src));
        }
    } else {
        return static_cast<Dst>(std::forward<Src>(src));
    }
}
//------------------------------------------------------------------------------
template <typename T, typename M>
struct convert;

template <
  typename T,
  std::size_t CI,
  std::size_t CppI,
  template <std::size_t, std::size_t>
  class Map>
struct convert<T, Map<CI, CppI>> {
    template <typename... P>
    constexpr auto operator()(size_constant<CI> i, P&&... p) const noexcept
      -> decltype(auto) {
        return c_arg_cast<T>(Map<CI, CppI>{}(i, std::forward<P>(p)...));
    }
};

template <
  typename T,
  std::size_t CI,
  std::size_t CppI,
  auto CS,
  template <std::size_t, std::size_t, auto>
  class Map>
struct convert<T, Map<CI, CppI, CS>> {
    template <typename... P>
    constexpr auto operator()(size_constant<CI> i, P&&... p) const noexcept
      -> decltype(auto) {
        return c_arg_cast<T>(Map<CI, CppI, CS>{}(i, std::forward<P>(p)...));
    }
};

template <typename T, std::size_t... J>
struct convert<T, trivial_arg_map<J...>> {
    template <std::size_t I, typename... P>
    constexpr auto operator()(size_constant<I> i, P&&... p) const noexcept
      -> decltype(auto) requires(... || (I == J)) {
        return c_arg_cast<T>(trivial_map{}(i, std::forward<P>(p)...));
    }
};
//------------------------------------------------------------------------------
template <typename... M>
struct combined_map : M... {
    using M::operator()...;
};
//------------------------------------------------------------------------------
template <std::size_t CI, std::size_t CppI, typename CP, typename CppP>
struct make_arg_map : convert<CP, reorder_arg_map<CI, CppI>> {};

template <std::size_t I, typename CP, typename CppP>
struct make_arg_map<I, I, CP, CppP> : convert<CP, trivial_arg_map<I>> {};

template <std::size_t CI, std::size_t CppI, typename P>
struct make_arg_map<CI, CppI, P, P> : reorder_arg_map<CI, CppI> {};

template <std::size_t I, typename P>
struct make_arg_map<I, I, P, P> : trivial_arg_map<I> {};

template <std::size_t I>
struct make_arg_map<I, I, const char*, string_view> {
    template <typename... P>
    constexpr auto operator()(size_constant<I> i, P&&... p) const noexcept {
        return c_str(trivial_map{}(i, std::forward<P>(p)...));
    }
};

template <std::size_t CI, std::size_t CppI, typename V, typename R, typename S>
struct make_arg_map<CI, CppI, V*, memory::basic_span<V, R, S>> {
    template <typename... P>
    constexpr auto operator()(size_constant<CI> i, P&&... p) const noexcept {
        return reorder_arg_map<CI, CppI>{}(i, std::forward<P>(p)...).data();
    }
};

template <std::size_t I, typename V, typename R, typename S>
struct make_arg_map<I, I, V*, memory::basic_span<V, R, S>> {
    template <typename... P>
    constexpr auto operator()(size_constant<I> i, P&&... p) const noexcept {
        return trivial_map{}(i, std::forward<P>(p)...).data();
    }
};

template <std::size_t CI, std::size_t CppI, typename V, typename R, typename S>
struct make_arg_map<CI, CppI, V*, memory::basic_split_span<V, R, S>> {
    template <typename... P>
    constexpr auto operator()(size_constant<CI> i, P&&... p) const noexcept {
        return reorder_arg_map<CI, CppI>{}(i, std::forward<P>(p)...)
          .tail()
          .data();
    }
};

template <std::size_t I, typename V, typename R, typename S>
struct make_arg_map<I, I, V*, memory::basic_split_span<V, R, S>> {
    template <typename... P>
    constexpr auto operator()(size_constant<I> i, P&&... p) const noexcept {
        return trivial_map{}(i, std::forward<P>(p)...).tail().data();
    }
};

template <std::size_t CI, std::size_t CppI>
struct make_arg_map<CI, CppI, void*, memory::block> {
    template <typename... P>
    constexpr auto operator()(size_constant<CI> i, P&&... p) const noexcept {
        return reorder_arg_map<CI, CppI>{}(i, std::forward<P>(p)...).data();
    }
};

template <std::size_t I>
struct make_arg_map<I, I, void*, memory::block> {
    template <typename... P>
    constexpr auto operator()(size_constant<I> i, P&&... p) const noexcept {
        return trivial_map{}(i, std::forward<P>(p)...).data();
    }
};

template <std::size_t CI, std::size_t CppI>
struct make_arg_map<CI, CppI, const void*, memory::const_block> {
    template <typename... P>
    constexpr auto operator()(size_constant<CI> i, P&&... p) const noexcept {
        return reorder_arg_map<CI, CppI>{}(i, std::forward<P>(p)...).data();
    }
};

template <std::size_t I>
struct make_arg_map<I, I, const void*, memory::const_block> {
    template <typename... P>
    constexpr auto operator()(size_constant<I> i, P&&... p) const noexcept {
        return trivial_map{}(i, std::forward<P>(p)...).data();
    }
};

template <std::size_t CI, std::size_t CppI, typename V, typename Tr>
requires(std::is_convertible_v<
         typename Tr::value_type*,
         V*>) struct make_arg_map<CI, CppI, V*, key_value_list<Tr>> {
    template <typename... P>
    constexpr auto operator()(size_constant<CI> i, P&&... p) const noexcept {
        return reorder_arg_map<CI, CppI>{}(i, std::forward<P>(p)...).data();
    }
};

template <std::size_t I, typename V, typename Tr>
requires(std::is_convertible_v<
         typename Tr::value_type*,
         V*>) struct make_arg_map<I, I, V*, key_value_list<Tr>> {
    template <typename... P>
    constexpr auto operator()(size_constant<I> i, P&&... p) const noexcept {
        return trivial_map{}(i, std::forward<P>(p)...).data();
    }
};

template <std::size_t I, typename Tag, typename Handle, Handle invalid>
struct make_arg_map<I, I, Handle, basic_handle<Tag, Handle, invalid>>
  : convert<Handle, trivial_arg_map<I>> {};

template <
  std::size_t CI,
  std::size_t CppI,
  typename Tag,
  typename Handle,
  Handle invalid>
struct make_arg_map<CI, CppI, Handle, basic_handle<Tag, Handle, invalid>>
  : convert<Handle, reorder_arg_map<CI, CppI>> {};

template <std::size_t I, typename Tag, typename Handle, Handle invalid>
struct make_arg_map<I, I, Handle, basic_owned_handle<Tag, Handle, invalid>> {
    template <typename... P>
    constexpr auto operator()(size_constant<I> i, P&&... p) const noexcept {
        return trivial_map{}(i, std::forward<P>(p)...).release();
    }
};

template <
  std::size_t CI,
  std::size_t CppI,
  typename Tag,
  typename Handle,
  Handle invalid>
struct make_arg_map<CI, CppI, Handle, basic_owned_handle<Tag, Handle, invalid>> {
    template <typename... P>
    constexpr auto operator()(size_constant<CI> i, P&&... p) const noexcept {
        return reorder_arg_map<CI, CppI>{}(i, std::forward<P>(p)...).release();
    }
};
//------------------------------------------------------------------------------
struct defaulted;
struct skipped;

template <auto>
struct substituted;

template <typename>
struct returned;

template <typename>
struct collapsed;

template <typename S, std::size_t CppSpanI, std::size_t CSizeI = 0>
struct head_transformed;

template <typename S, std::size_t CppSpanI, std::size_t CSizeI = 0>
struct skip_transformed;

template <typename S, std::size_t CppSpanI, std::size_t CSizeI = 0>
struct split_transformed;

template <std::size_t CI, std::size_t CppI, typename CP, typename CppP>
struct make_args_map;

template <std::size_t CI, std::size_t CppI>
struct make_args_map<CI, CppI, mp_list<>, mp_list<>> {
protected:
    void operator()() const noexcept {};
};

template <
  std::size_t CI,
  std::size_t CppI,
  typename CH,
  typename... CT,
  typename CppH,
  typename... CppT>
struct make_args_map<CI, CppI, mp_list<CH, CT...>, mp_list<CppH, CppT...>>
  : make_arg_map<
      CI,
      CppI,
      std::remove_cv_t<CH>,
      std::remove_cv_t<std::remove_reference_t<CppH>>>
  , make_args_map<CI + 1, CppI + 1, mp_list<CT...>, mp_list<CppT...>> {
    using make_arg_map<
      CI,
      CppI,
      std::remove_cv_t<CH>,
      std::remove_cv_t<std::remove_reference_t<CppH>>>::operator();
    using make_args_map<CI + 1, CppI + 1, mp_list<CT...>, mp_list<CppT...>>::
    operator();
};

template <
  std::size_t CI,
  std::size_t CppI,
  typename CH,
  typename... CT,
  typename... CppT>
struct make_args_map<CI, CppI, mp_list<CH, CT...>, mp_list<skipped, CppT...>>
  : make_args_map<CI + 1, CppI, mp_list<CT...>, mp_list<CppT...>> {
    using make_args_map<CI + 1, CppI, mp_list<CT...>, mp_list<CppT...>>::
    operator();
};

template <
  std::size_t CI,
  std::size_t CppI,
  typename CH,
  typename... CT,
  typename... CppT>
struct make_args_map<CI, CppI, mp_list<CH, CT...>, mp_list<defaulted, CppT...>>
  : make_args_map<CI + 1, CppI, mp_list<CT...>, mp_list<CppT...>> {
    using make_args_map<CI + 1, CppI, mp_list<CT...>, mp_list<CppT...>>::
    operator();

    template <typename... P>
    constexpr auto operator()(size_constant<CI>, P&&...) const noexcept -> CH {
        if constexpr(std::is_pointer_v<CH>) {
            return nullptr;
        } else if constexpr(std::is_integral_v<CH>) {
            return static_cast<CH>(0);
        } else {
            return {};
        }
    }
};

template <
  std::size_t CI,
  std::size_t CppI,
  typename CH,
  typename... CT,
  auto value,
  typename... CppT>
requires(std::is_convertible_v<decltype(value), CH>) struct make_args_map<
  CI,
  CppI,
  mp_list<CH, CT...>,
  mp_list<substituted<value>, CppT...>>
  : make_args_map<CI + 1, CppI, mp_list<CT...>, mp_list<CppT...>> {
    using make_args_map<CI + 1, CppI, mp_list<CT...>, mp_list<CppT...>>::
    operator();

    template <typename... P>
    constexpr auto operator()(size_constant<CI>, P&&...) const noexcept {
        return value;
    }
};

template <
  std::size_t CI,
  std::size_t CppI,
  typename CH,
  typename... CT,
  typename CppH,
  typename... CppT>
requires(std::is_convertible_v<CH, CppH>) struct make_args_map<
  CI,
  CppI,
  mp_list<CH*, CT...>,
  mp_list<returned<CppH>, CppT...>>
  : make_args_map<CI + 1, CppI, mp_list<CT...>, mp_list<CppT...>> {
    using make_args_map<CI + 1, CppI, mp_list<CT...>, mp_list<CppT...>>::
    operator();

    template <typename... P>
    constexpr auto operator()(size_constant<0> i, P&&... p) const noexcept {
        return trivial_map{}(i, std::forward<P>(p)...)
          .replaced_with(_value)
          .cast_to(type_identity<CppH>{});
    }

    template <typename... P>
    constexpr auto operator()(size_constant<CI>, P&&...) noexcept -> CH* {
        return &_value;
    }

private:
    CH _value{};
};

template <
  std::size_t CI,
  std::size_t CppI,
  typename Tag,
  typename Handle,
  Handle invalid,
  typename... CT,
  typename... CppT>
struct make_args_map<
  CI,
  CppI,
  mp_list<Handle*, CT...>,
  mp_list<returned<basic_handle<Tag, Handle, invalid>>, CppT...>>
  : make_args_map<CI + 1, CppI, mp_list<CT...>, mp_list<CppT...>> {
    using make_args_map<CI + 1, CppI, mp_list<CT...>, mp_list<CppT...>>::
    operator();

    template <typename... P>
    constexpr auto operator()(size_constant<0> i, P&&... p) const noexcept {
        return trivial_map{}(i, std::forward<P>(p)...)
          .replaced_with(basic_handle<Tag, Handle, invalid>{_value});
    }

    template <typename... P>
    constexpr auto operator()(size_constant<CI>, P&&...) noexcept -> Handle* {
        return &_value;
    }

private:
    Handle _value{};
};

template <
  std::size_t CI,
  std::size_t CppI,
  typename H,
  typename... CT,
  typename... CppT>
struct make_args_map<CI, CppI, mp_list<H*, CT...>, mp_list<H&, CppT...>>
  : addressof_map<CI, CppI>
  , make_args_map<CI + 1, CppI + 1, mp_list<CT...>, mp_list<CppT...>> {
    using addressof_map<CI, CppI>::operator();
    using make_args_map<CI + 1, CppI + 1, mp_list<CT...>, mp_list<CppT...>>::
    operator();
};

template <std::size_t CI, std::size_t CppI, typename CH>
requires(
  std::is_pointer_v<CH>) struct make_args_map<CI, CppI, mp_list<CH>, mp_list<>>
  : nullptr_arg_map<CI> {
    using nullptr_arg_map<CI>::operator();
};

template <
  std::size_t CI,
  std::size_t CppI,
  typename CP,
  typename CS,
  typename... CT,
  typename CppV,
  typename CppP,
  typename CppS,
  typename... CppT>
requires(std::is_pointer_v<CP>&& std::is_convertible_v<CppP, CP>&&
           std::is_integral_v<CS>&& std::is_convertible_v<CppS, CS>) struct
  make_args_map<
    CI,
    CppI,
    mp_list<CP, CS, CT...>,
    mp_list<memory::basic_span<CppV, CppP, CppS>, CppT...>>
  : convert<CP, get_data_map<CI, CppI>>
  , convert<CS, get_size_map<CI + 1, CppI>>
  , make_args_map<CI + 2, CppI + 1, mp_list<CT...>, mp_list<CppT...>> {
    using convert<CP, get_data_map<CI, CppI>>::operator();
    using convert<CS, get_size_map<CI + 1, CppI>>::operator();
    using make_args_map<CI + 2, CppI + 1, mp_list<CT...>, mp_list<CppT...>>::
    operator();
};

template <
  std::size_t CI,
  std::size_t CppI,
  typename CS,
  typename CP,
  typename... CT,
  typename CppV,
  typename CppP,
  typename CppS,
  typename... CppT>
requires(std::is_pointer_v<CP>&& std::is_convertible_v<CppP, CP>&&
           std::is_integral_v<CS>&& std::is_convertible_v<CppS, CS>) struct
  make_args_map<
    CI,
    CppI,
    mp_list<CS, CP, CT...>,
    mp_list<memory::basic_span<CppV, CppP, CppS>, CppT...>>
  : convert<CS, get_size_map<CI, CppI>>
  , convert<CP, get_data_map<CI + 1, CppI>>
  , make_args_map<CI + 2, CppI + 1, mp_list<CT...>, mp_list<CppT...>> {
    using convert<CS, get_size_map<CI, CppI>>::operator();
    using convert<CP, get_data_map<CI + 1, CppI>>::operator();
    using make_args_map<CI + 2, CppI + 1, mp_list<CT...>, mp_list<CppT...>>::
    operator();
};

template <
  std::size_t CI,
  std::size_t CppI,
  typename CS,
  typename CP,
  typename... CT,
  typename CppV,
  typename CppP,
  typename CppS,
  CppS chunkSize,
  typename... CppT>
requires(std::is_pointer_v<CP>&& std::is_convertible_v<CppP, CP>&&
           std::is_integral_v<CS>&& std::is_convertible_v<CppS, CS>) struct
  make_args_map<
    CI,
    CppI,
    mp_list<CS, CP, CT...>,
    mp_list<memory::basic_chunk_span<CppV, CppP, CppS, chunkSize>, CppT...>>
  : convert<CS, get_chunk_size_map<CI, CppI, chunkSize>>
  , convert<CP, get_data_map<CI + 1, CppI>>
  , make_args_map<CI + 2, CppI + 1, mp_list<CT...>, mp_list<CppT...>> {
    using convert<CS, get_chunk_size_map<CI, CppI, chunkSize>>::operator();
    using convert<CP, get_data_map<CI + 1, CppI>>::operator();
    using make_args_map<CI + 2, CppI + 1, mp_list<CT...>, mp_list<CppT...>>::
    operator();
};

template <
  std::size_t CI,
  std::size_t CppI,
  typename CS,
  typename CH,
  typename... CT,
  typename Tag,
  CH invalid,
  typename... CppT>
requires(std::is_integral_v<CS>) struct make_args_map<
  CI,
  CppI,
  mp_list<CS, CH*, CT...>,
  mp_list<basic_handle<Tag, CH, invalid>, CppT...>>
  : make_args_map<CI + 2, CppI + 1, mp_list<CT...>, mp_list<CppT...>> {
    using make_args_map<CI + 2, CppI + 1, mp_list<CT...>, mp_list<CppT...>>::
    operator();

    template <typename... P>
    constexpr auto operator()(size_constant<CI>, P&&...) const noexcept {
        return static_cast<CS>(1);
    }

    template <typename... P>
    constexpr auto operator()(size_constant<CI + 1> i, P&&... p) const noexcept {
        return static_cast<CH*>(
          reorder_arg_map<CI + 1, CppI>{}(i, std::forward<P>(p)...));
    }
};

template <
  std::size_t CI,
  std::size_t CppI,
  typename CS,
  typename CH,
  typename... CT,
  typename Tag,
  CH invalid,
  typename... CppT>
requires(std::is_integral_v<CS>) struct make_args_map<
  CI,
  CppI,
  mp_list<CS, const CH*, CT...>,
  mp_list<basic_handle<Tag, CH, invalid>, CppT...>>
  : make_args_map<CI + 2, CppI + 1, mp_list<CT...>, mp_list<CppT...>> {
    using make_args_map<CI + 2, CppI + 1, mp_list<CT...>, mp_list<CppT...>>::
    operator();

    template <typename... P>
    constexpr auto operator()(size_constant<CI>, P&&...) const noexcept {
        return static_cast<CS>(1);
    }

    template <typename... P>
    constexpr auto operator()(size_constant<CI + 1> i, P&&... p) const noexcept {
        return static_cast<const CH*>(
          reorder_arg_map<CI + 1, CppI>{}(i, std::forward<P>(p)...));
    }
};

template <
  std::size_t CI,
  std::size_t CppI,
  typename CS,
  typename CH,
  typename... CT,
  typename Tag,
  CH invalid,
  typename... CppT>
requires(std::is_integral_v<CS>) struct make_args_map<
  CI,
  CppI,
  mp_list<CS, const CH*, CT...>,
  mp_list<basic_owned_handle<Tag, CH, invalid>, CppT...>>
  : make_args_map<CI + 2, CppI + 1, mp_list<CT...>, mp_list<CppT...>> {
    using make_args_map<CI + 2, CppI + 1, mp_list<CT...>, mp_list<CppT...>>::
    operator();

    template <typename... P>
    constexpr auto operator()(size_constant<CI>, P&&...) const noexcept {
        return static_cast<CS>(1);
    }

    template <typename... P>
    constexpr auto operator()(size_constant<CI + 1> i, P&&... p) noexcept {
        _handle =
          reorder_arg_map<CI + 1, CppI>{}(i, std::forward<P>(p)...).release();
        return &_handle;
    }

private:
    CH _handle{invalid};
};

template <
  std::size_t CI,
  std::size_t CppI,
  typename CP,
  typename CS,
  typename... CT,
  typename CppV,
  typename CppP,
  typename CppS,
  typename... CppT>
requires(std::is_pointer_v<CP>&& std::is_convertible_v<CppP, CP>&&
           std::is_integral_v<CS>&& std::is_convertible_v<CppS, CS>) struct
  make_args_map<
    CI,
    CppI,
    mp_list<CP, CS, CT...>,
    mp_list<basic_string_span<CppV, CppP, CppS>, CppT...>>
  : convert<CP, get_data_map<CI, CppI>>
  , convert<CS, get_size_map<CI + 1, CppI>>
  , make_args_map<CI + 2, CppI + 1, mp_list<CT...>, mp_list<CppT...>> {
    using convert<CP, get_data_map<CI, CppI>>::operator();
    using convert<CS, get_size_map<CI + 1, CppI>>::operator();
    using make_args_map<CI + 2, CppI + 1, mp_list<CT...>, mp_list<CppT...>>::
    operator();
};

template <
  std::size_t CI,
  std::size_t CppI,
  typename CS,
  typename CP,
  typename... CT,
  typename CppV,
  typename CppP,
  typename CppS,
  typename... CppT>
requires(std::is_pointer_v<CP>&& std::is_convertible_v<CppP, CP>&&
           std::is_integral_v<CS>&& std::is_convertible_v<CppS, CS>) struct
  make_args_map<
    CI,
    CppI,
    mp_list<CS, CP, CT...>,
    mp_list<basic_string_span<CppV, CppP, CppS>, CppT...>>
  : convert<CS, get_size_map<CI, CppI>>
  , convert<CP, get_data_map<CI + 1, CppI>>
  , make_args_map<CI + 2, CppI + 1, mp_list<CT...>, mp_list<CppT...>> {
    using convert<CS, get_size_map<CI, CppI>>::operator();
    using convert<CP, get_data_map<CI + 1, CppI>>::operator();
    using make_args_map<CI + 2, CppI + 1, mp_list<CT...>, mp_list<CppT...>>::
    operator();
};

template <
  std::size_t CI,
  std::size_t CppI,
  typename CS,
  typename... CT,
  typename CppS,
  typename... CppT>
requires(std::is_integral_v<CS>&& std::is_convertible_v<CppS, CS>) struct make_args_map<
  CI,
  CppI,
  mp_list<CS, const void*, CT...>,
  mp_list<buffer_data_spec<CppS>, CppT...>>
  : convert<CS, get_size_map<CI, CppI>>
  , get_data_map<CI + 1, CppI>
  , make_args_map<CI + 2, CppI + 1, mp_list<CT...>, mp_list<CppT...>> {
    using convert<CS, get_size_map<CI, CppI>>::operator();
    using get_data_map<CI + 1, CppI>::operator();
    using make_args_map<CI + 2, CppI + 1, mp_list<CT...>, mp_list<CppT...>>::
    operator();
};
//------------------------------------------------------------------------------
template <typename CSignature, typename CppSignature>
static auto make_map(CSignature*, CppSignature*) -> trivial_map;

template <typename CRV, typename CppRV>
static auto make_map(CRV (*)(), CppRV (*)())
  -> cast_to_map<CRV, CppRV> requires(!std::is_same_v<CRV, CppRV>);

template <typename RV, typename... CParam, typename... CppParam>
static auto make_map(RV (*)(CParam...), RV (*)(CppParam...))
  -> make_args_map<0, 0, mp_list<RV, CParam...>, mp_list<RV, CppParam...>> requires(
    (sizeof...(CParam) > 0));

template <typename CRV, typename CppRV, typename... CParam, typename... CppParam>
static auto make_map(CRV (*)(CParam...), CppRV (*)(CppParam...)) -> combined_map<
  cast_to_map<CRV, CppRV>,
  make_args_map<
    1,
    1,
    mp_list<CParam...>,
    mp_list<
      CppParam...>>> requires(!std::is_same_v<CRV, CppRV> && (sizeof...(CParam) > 0));

template <typename CRV, typename CppRV, typename CParam, typename CppParam>
static auto make_map(CRV (*)(CppRV*, CParam), CppRV (*)(CppParam))
  -> combined_map<
    get_ptr_arg_map<1, CppRV>,
    convert<CParam, reorder_arg_map<2, 1>>>;

template <typename CRV, typename CppRV, typename... CParam, typename... CppParam>
static auto make_map(CRV (*)(CParam...), returned<CppRV> (*)(CppParam...))
  -> make_args_map<1, 1, mp_list<CParam...>, mp_list<CppParam...>>;

template <typename RV, typename... CParam, typename... CppParam>
static auto make_map(RV (*)(CParam...), collapsed<RV> (*)(CppParam...))
  -> combined_map<
    collapse_map,
    make_args_map<1, 1, mp_list<CParam...>, mp_list<CppParam...>>>;

template <typename RV>
static auto make_map(RV (*)(), collapsed<RV> (*)()) -> collapse_map;

template <
  typename CRV,
  typename HTS,
  std::size_t CppSpanI,
  std::size_t CSizeI,
  typename... CParam,
  typename... CppParam>
static auto make_map(
  CRV (*)(CParam...),
  head_transformed<HTS, CppSpanI, CSizeI> (*)(CppParam...))
  -> combined_map<
    head_transform_map<HTS, CppSpanI, CSizeI>,
    make_args_map<1, 1, mp_list<CParam...>, mp_list<CppParam...>>>;

template <
  typename CRV,
  typename HTS,
  std::size_t CppSpanI,
  std::size_t CSizeI,
  typename... CParam,
  typename... CppParam>
static auto make_map(
  CRV (*)(CParam...),
  skip_transformed<HTS, CppSpanI, CSizeI> (*)(CppParam...))
  -> combined_map<
    skip_transform_map<HTS, CppSpanI, CSizeI>,
    make_args_map<1, 1, mp_list<CParam...>, mp_list<CppParam...>>>;

template <
  typename CRV,
  typename STS,
  std::size_t CppSpanI,
  std::size_t CSizeI,
  typename... CParam,
  typename... CppParam>
static auto make_map(
  CRV (*)(CParam...),
  split_transformed<STS, CppSpanI, CSizeI> (*)(CppParam...))
  -> combined_map<
    split_transform_map<STS, CppSpanI, CSizeI>,
    make_args_map<1, 1, mp_list<CParam...>, mp_list<CppParam...>>>;

template <typename CSignature, typename CppSignature>
using make_map_t = decltype(make_map(
  static_cast<CSignature*>(nullptr),
  static_cast<CppSignature*>(nullptr)));
//------------------------------------------------------------------------------
} // namespace eagine::c_api

#endif

