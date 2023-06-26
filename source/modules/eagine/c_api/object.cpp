/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.c_api:object;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.container;

namespace eagine::c_api {
//------------------------------------------------------------------------------
/// @brief Non-owning wrapper for C-API opaque handle types.
/// @ingroup c_api_wrap
/// @tparam Tag type distinguishing various handler with the same underlying type.
/// @tparam Handle the underlying handle type.
/// @tparam invalid the invalid value for this particular handle type.
/// @see basic_owned_handle
export template <typename Tag, typename Handle, Handle invalid = ~Handle(0)>
class basic_handle {
public:
    /// @brief Alias for the tag type.
    using tag_type = Tag;

    /// @brief Alias for the underlying handle type.
    using handle_type = Handle;

    ~basic_handle() noexcept = default;

    /// @brief Default constructor.
    /// @post not has_value()
    constexpr basic_handle() noexcept = default;

    /// @brief Move constructor.
    constexpr basic_handle(basic_handle&& tmp) noexcept
      : _name{tmp._release()} {}

    /// @brief Move assignment operator.
    auto operator=(basic_handle&& tmp) noexcept -> auto& {
        _name = tmp._release();
        return *this;
    }

    /// @brief Copy constructor.
    constexpr basic_handle(const basic_handle&) noexcept = default;

    /// @brief Copy assignment operator.
    auto operator=(const basic_handle&) noexcept -> basic_handle& = default;

    /// @brief Construction from the underlying handle type.
    explicit constexpr basic_handle(Handle name) noexcept
      : _name{name} {}

    /// @brief Indicates if this instance contains a valid handle.
    constexpr auto has_value() const noexcept -> bool {
        return _name != invalid;
    }

    /// @brief Indicates if this instance contains a valid handle.
    /// @see has_value
    explicit constexpr operator bool() const noexcept {
        return has_value();
    }

    /// @brief Dereference
    constexpr auto operator*() const noexcept -> Handle {
        return _name;
    }

    /// @brief Explicit conversion to the underlying handle type.
    explicit constexpr operator Handle() const noexcept {
        return _name;
    }

    /// @brief Explicit conversion to a pointer to the underlying handle type.
    explicit constexpr operator Handle*() noexcept {
        return &_name;
    }

    struct transform {
        constexpr auto operator()(Handle hndl) const noexcept {
            return basic_handle<Tag, Handle, invalid>{hndl};
        }
    };

protected:
    auto _release() noexcept {
        return std::exchange(_name, invalid);
    }

private:
    Handle _name{invalid};
};
//------------------------------------------------------------------------------
export template <typename Tag, typename Handle, Handle invalid>
constexpr auto to_underlying(basic_handle<Tag, Handle, invalid> h) noexcept
  -> Handle {
    return static_cast<Handle>(h);
}
//------------------------------------------------------------------------------
/// @brief Owning wrapper for C-API opaque handle types.
/// @ingroup c_api_wrap
/// @tparam Tag type distinguisihing various handler with the same underlying type.
/// @tparam Handle the underlying handle type.
/// @tparam invalid the invalid value for this particular handle type.
/// @see basic_handle
/// @see object
export template <typename Tag, typename Handle, Handle invalid = ~Handle(0)>
class basic_owned_handle : public basic_handle<Tag, Handle, invalid> {
    using base = basic_handle<Tag, Handle, invalid>;

public:
    ~basic_owned_handle() noexcept = default;

    /// @brief Default constructor.
    /// @post not has_value()
    constexpr basic_owned_handle() noexcept = default;

    /// @brief Move constructor.
    constexpr basic_owned_handle(basic_owned_handle&& tmp) noexcept
      : base{tmp.release()} {}

    /// @brief Move assignment operator.
    auto operator=(basic_owned_handle&& tmp) noexcept -> auto& {
        *static_cast<base*>(this) = static_cast<base&&>(tmp);
        return *this;
    }

    /// @brief Copy constructor.
    basic_owned_handle(const basic_owned_handle&) = delete;

    /// @brief Copy assignment operator.
    auto operator=(const basic_owned_handle&) = delete;

    /// @brief Constructor adopting a non-owning handle wrapper.
    explicit constexpr basic_owned_handle(base adopted) noexcept
      : base{adopted} {}

    /// @brief Constructor adopting an underlying handle value.
    explicit constexpr basic_owned_handle(Handle name) noexcept
      : base{name} {}

    /// @brief Releases the underlying handle value.
    /// @post not has_value()
    auto release() noexcept -> Handle {
        return this->_release();
    }
};
//------------------------------------------------------------------------------
export template <typename BasicHandle, typename Container>
class basic_handle_container;

/// @brief Basic template for C-API opaque handle containers and ranges.
/// @ingroup c_api_wrap
export template <typename Tag, typename Handle, Handle invalid, typename Container>
class basic_handle_container<basic_handle<Tag, Handle, invalid>, Container>
  : public basic_wrapping_container<
      Container,
      basic_handle<Tag, Handle, invalid>,
      Handle,
      invalid> {
    using base = basic_wrapping_container<
      Container,
      basic_handle<Tag, Handle, invalid>,
      Handle,
      invalid>;

public:
    using base::base;

    /// @brief Returns the raw handles.
    constexpr auto raw_handles() noexcept {
        return this->raw_items();
    }

    /// @brief Returns the raw handles.
    constexpr auto raw_handles() const noexcept {
        return this->raw_items();
    }
};
//------------------------------------------------------------------------------
/// @brief Alias for basic handle container based on non-const span.
/// @ingroup c_api_wrap
/// @see basic_handle_view
/// @see basic_handle_array
/// @see basic_handle_vector
export template <typename BasicHandle>
using basic_handle_span =
  basic_handle_container<BasicHandle, span<typename BasicHandle::handle_type>>;

/// @brief Alias for basic handle container based on const span.
/// @ingroup c_api_wrap
/// @see basic_handle_span
/// @see basic_handle_array
/// @see basic_handle_vector
export template <typename BasicHandle>
using basic_handle_view = basic_handle_container<
  BasicHandle,
  span<const typename BasicHandle::handle_type>>;

/// @brief Alias for basic handle container based on std::array.
/// @ingroup c_api_wrap
/// @see basic_handle_span
/// @see basic_handle_view
/// @see basic_handle_vector
export template <typename BasicHandle, std::size_t N>
using basic_handle_array = basic_handle_container<
  BasicHandle,
  std::array<typename BasicHandle::handle_type, N>>;

/// @brief Alias for basic handle container based on std::vector.
/// @ingroup c_api_wrap
/// @see basic_handle_span
/// @see basic_handle_view
/// @see basic_handle_array
export template <typename BasicHandle>
using basic_handle_vector = basic_handle_container<
  BasicHandle,
  std::vector<typename BasicHandle::handle_type>>;
//------------------------------------------------------------------------------
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
    /// @post not has_value()
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
        if(this->has_value()) {
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
//------------------------------------------------------------------------------
} // namespace eagine::c_api
