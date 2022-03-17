/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_C_API_API_TRAITS_HPP
#define EAGINE_C_API_API_TRAITS_HPP

#include "../string_span.hpp"
#include "../type_identity.hpp"
#include "../valid_if/always.hpp"
#include "../valid_if/never.hpp"

namespace eagine::c_api {
//------------------------------------------------------------------------------
/// @brief Policy class customizing C-API constant value loading and function linking.
/// @ingroup c_api_wrap
struct default_traits {
    template <typename Tag, typename Signature>
    using function_pointer = std::add_pointer<Signature>;

    template <typename Api, typename Type>
    auto load_constant(Api&, const string_view, const type_identity<Type>)
      -> std::tuple<Type, bool> {
        return {{}, false};
    }

    template <typename Api, typename Tag, typename Signature>
    auto link_function(
      Api&,
      const Tag,
      const string_view,
      const type_identity<Signature>) -> std::add_pointer_t<Signature> {
        return nullptr;
    }

    template <typename Tag, typename RV>
    static constexpr auto fallback(const Tag, const type_identity<RV>) -> RV {
        return {};
    }

    template <typename Tag>
    static constexpr void fallback(const Tag, const type_identity<void>) {}

    template <typename RV, typename Tag, typename... Params, typename... Args>
    static constexpr auto call_static(
      const Tag tag,
      RV (*function)(Params...),
      Args&&... args) -> RV {
        if(function) {
            return function(
              std::forward<Args>(args)...); // NOLINT(hicpp-no-array-decay)
        }
        return fallback(tag, type_identity<RV>());
    }

    template <typename RV, typename Tag, typename... Params, typename... Args>
    static constexpr auto call_dynamic(
      const Tag tag,
      RV (*function)(Params...),
      Args&&... args) -> RV {
        if(function) {
            return function(
              std::forward<Args>(args)...); // NOLINT(hicpp-no-array-decay)
        }
        return fallback(tag, type_identity<RV>());
    }

    template <typename Api, typename Result>
    static constexpr auto check_result(const Api&, Result result) noexcept {
        return result;
    }

    template <typename RV>
    using no_result = never_valid<RV>;

    template <typename RV>
    using opt_result = optionally_valid<RV>;

    template <typename RV>
    using result = always_valid<RV>;
};
//------------------------------------------------------------------------------
} // namespace eagine::c_api

#endif
