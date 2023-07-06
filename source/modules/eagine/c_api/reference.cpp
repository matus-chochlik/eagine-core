/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.c_api:reference;

import std;
import eagine.core.types;

namespace eagine::c_api {
//------------------------------------------------------------------------------
export template <
  typename ApiWrapper,
  typename OpsWrapper = typename ApiWrapper::operations_type,
  typename ValWrapper = typename ApiWrapper::constants_type>
class basic_api_reference : public optional_reference<ApiWrapper> {
public:
    using optional_reference<ApiWrapper>::optional_reference;

    template <typename Function>
    auto then(Function&& function) const -> bool {
        if(this->has_value()) [[likely]] {
            const auto& api = *(*this);
            const auto& [functions, constants] = api;
            std::forward<Function>(function)(functions, constants, api);
            return true;
        }
        return false;
    }

    template <typename Class>
    auto then(
      Class* that,
      void (Class::*member_function)(
        const OpsWrapper&,
        const ValWrapper&,
        const ApiWrapper&)) const {
        if(this->has_value()) [[likely]] {
            const auto& api = *(*this);
            const auto& [operations, constants] = api;
            (that->*member_function)(operations, constants, api);
            return true;
        }
        return false;
    }

    template <typename Class>
    auto then(
      Class* that,
      void (Class::*member_function)(
        const OpsWrapper&,
        const ValWrapper&,
        const ApiWrapper&) noexcept) const noexcept {
        if(this->has_value()) [[likely]] {
            const auto& api = *(*this);
            const auto& [operations, constants] = api;
            (that->*member_function)(operations, constants, api);
            return true;
        }
        return false;
    }

    template <typename Class>
    auto then(
      const Class* that,
      void (Class::*member_function)(
        const OpsWrapper&,
        const ValWrapper&,
        const ApiWrapper&) const) const {
        if(this->has_value()) [[likely]] {
            const auto& api = *(*this);
            const auto& [operations, constants] = api;
            (that->*member_function)(operations, constants, api);
            return true;
        }
        return false;
    }

    template <typename Class>
    auto then(
      const Class* that,
      void (Class::*member_function)(
        const OpsWrapper&,
        const ValWrapper&,
        const ApiWrapper&) const noexcept) const noexcept {
        if(this->has_value()) [[likely]] {
            const auto& api = *(*this);
            const auto& [operations, constants] = api;
            (that->*member_function)(operations, constants, api);
            return true;
        }
        return false;
    }
};
//------------------------------------------------------------------------------
} // namespace eagine::c_api
