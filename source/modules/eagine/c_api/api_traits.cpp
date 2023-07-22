/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.c_api:api_traits;

import std;
import eagine.core.types;
import eagine.core.memory;

namespace eagine::c_api {
//------------------------------------------------------------------------------
/// @brief Policy class customizing C-API constant value loading and function linking.
/// @ingroup c_api_wrap
export struct default_traits {
    template <typename Tag, typename Signature>
    using function_pointer = std::add_pointer_t<Signature>;

    template <typename Api, typename Type>
    auto load_constant(Api&, const string_view, const std::type_identity<Type>)
      -> std::tuple<Type, bool> {
        return {{}, false};
    }

    template <typename Api, typename Tag, typename Signature>
    auto link_function(
      Api&,
      const Tag,
      const string_view,
      const std::type_identity<Signature>) -> std::add_pointer_t<Signature> {
        return nullptr;
    }

    template <typename RV>
    using adapt_rv = std::conditional_t<
      std::is_reference_v<RV>,
      optional_reference<std::remove_reference_t<RV>>,
      RV>;

    template <typename Tag, typename RV>
    static constexpr auto fallback(const Tag, const std::type_identity<RV>)
      -> adapt_rv<RV> {
        return {};
    }

    template <typename Tag>
    static constexpr void fallback(const Tag, const std::type_identity<void>) {}

    template <typename RV, typename Tag, typename... Params, typename... Args>
    static constexpr auto call_static(
      const Tag tag,
      RV (*function)(Params...),
      Args&&... args) -> adapt_rv<RV> {
        if(function) {
            return function(
              std::forward<Args>(args)...); // NOLINT(hicpp-no-array-decay)
        }
        return fallback(tag, std::type_identity<RV>());
    }

    template <typename RV, typename Tag, typename... Params, typename... Args>
    static constexpr auto call_dynamic(
      const Tag tag,
      RV (*function)(Params...),
      Args&&... args) -> adapt_rv<RV> {
        if(function) {
            return function(
              std::forward<Args>(args)...); // NOLINT(hicpp-no-array-decay)
        }
        return fallback(tag, std::type_identity<RV>());
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
