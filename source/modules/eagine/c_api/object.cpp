/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.c_api:object;

import eagine.core.types;
import eagine.core.memory;
import eagine.core.container;
import :handle;
import <tuple>;

namespace eagine::c_api {

/// @brief Owning wrapper for C-API opaque handle types.
/// @ingroup c_api_wrap
/// @tparam Api the API instance which will be used to clean up this object.
/// @tparam Tag type distinguishing various handles with the same underlying type.
/// @tparam Handle the underlying handle type.
/// @tparam invalid the invalid value for this particular handle type.
/// @see basic_handle
/// @see basic_owned_handle
export template <
  typename Api,
  typename Tag,
  typename Handle,
  Handle invalid,
  typename... P>
class basic_object : public basic_owned_handle<Tag, Handle, invalid> {
    using base = basic_owned_handle<Tag, Handle, invalid>;

public:
    /// @brief Default constructor.
    /// @post not is_valid()
    constexpr basic_object() noexcept = default;

    /// @brief Move constructor.
    constexpr basic_object(basic_object&&) noexcept = default;

    /// @brief No copy construction.
    basic_object(const basic_object&) = delete;

    /// @brief Move assignment.
    constexpr auto operator=(basic_object&&) noexcept
      -> basic_object& = default;

    /// @brief No copy assignment.
    auto operator=(const basic_object&) = delete;

    /// @brief Initializing constructor.
    constexpr basic_object(const Api& api, base init, P... args) noexcept
      : base{std::move(init)}
      , _api{api}
      , _args{args...} {}

    /// @brief Uses the associated Api to clean up this object.
    ~basic_object() noexcept {
        if(this->is_valid()) {
            _clean_up(std::make_index_sequence<sizeof...(P)>{});
        }
    }

private:
    template <std::size_t... I>
    void _clean_up(std::index_sequence<I...>) noexcept {
        try {
            _api.clean_up(static_cast<base&&>(*this), std::get<I>(_args)...);
        } catch(...) {
        }
    }

    const Api& _api;
    [[no_unique_address]] std::tuple<P...> _args;
};

template <typename Api, typename Handle, typename... P>
struct get_basic_object_from_handle;

export template <typename Api, typename Handle, typename... P>
using basic_object_from_handle_t =
  typename get_basic_object_from_handle<Api, Handle, P...>::type;

template <typename Api, typename Tag, typename Handle, Handle invalid, typename... P>
struct get_basic_object_from_handle<Api, basic_handle<Tag, Handle, invalid>, P...>
  : std::type_identity<basic_object<Api, Tag, Handle, invalid, P...>> {};

} // namespace eagine::c_api
