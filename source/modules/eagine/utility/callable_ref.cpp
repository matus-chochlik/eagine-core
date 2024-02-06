/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.utility:callable_ref;

import std;
import eagine.core.types;

namespace eagine {
//------------------------------------------------------------------------------
export template <typename Sig>
struct is_noexcept_function;

export template <typename Sig>
using is_noexcept_function_t = type_t<is_noexcept_function<Sig>>;

export template <typename Sig>
inline constexpr bool is_noexcept_function_v = is_noexcept_function<Sig>::value;

export template <typename RV, typename... P>
struct is_noexcept_function<RV(P...)> : std::false_type {};

export template <typename RV, typename... P>
struct is_noexcept_function<RV(P...) noexcept> : std::true_type {};
//------------------------------------------------------------------------------
/// @brief Declaration of class template storing a reference to a callable object.
/// @ingroup functional
/// @see callable_ref
export template <typename FuncSig, bool NoExcept>
class basic_callable_ref;

/// @brief Implementation of class template storing a reference to a callable object.
/// @ingroup functional
/// @see callable_ref
///
/// Unlike std::function basic_callable_ref does not do any conversion on
/// the parameters or return values, the referenced callable must match,
/// and it does not copy the callable, it must remain in scope while the
/// reference is used.
export template <typename RV, typename... P, bool NE>
class basic_callable_ref<RV(P...) noexcept(NE), NE> {
public:
    /// @brief Alias for the callable's argument type tuple.
    using argument_tuple_type =
      std::tuple<std::remove_cv_t<std::remove_reference_t<P>>...>;

    /// @brief Creates a default-constructed instance of argument_type_tuple.
    static auto argument_tuple() noexcept -> argument_tuple_type {
        return {};
    }

    /// @brief Default constructor.
    /// @post not is_valid
    constexpr basic_callable_ref() noexcept = default;

    /// @brief Move constructor.
    constexpr basic_callable_ref(basic_callable_ref&& temp) noexcept
      : _data{std::exchange(temp._data, nullptr)}
      , _func{std::exchange(temp._func, nullptr)} {}

    /// @brief Copy constructor.
    constexpr basic_callable_ref(const basic_callable_ref&) noexcept = default;

    /// @brief Move assignment operator.
    constexpr auto operator=(basic_callable_ref&& temp) noexcept
      -> basic_callable_ref& {
        std::swap(temp._data, _data);
        std::swap(temp._func, _func);
        return *this;
    }

    /// @brief Copy assignment operator.
    constexpr auto operator=(const basic_callable_ref&) noexcept
      -> basic_callable_ref& = default;

    ~basic_callable_ref() noexcept = default;

    /// @brief Construction from a pointer to function.
    constexpr basic_callable_ref(RV (*func)(P...) noexcept(NE)) noexcept
      : _func{reinterpret_cast<_func_t>(func)} {}

    /// @brief Construction from a reference to an object and a pointer to function.
    template <typename C>
    constexpr basic_callable_ref(
      optional_reference<C> data,
      RV (*func)(C*, P...) noexcept(NE)) noexcept
      : _data{static_cast<void*>(data.get())}
      , _func{reinterpret_cast<_func_t>(func)} {
        assert(_data != nullptr);
    }

    /// @brief Construction a reference to object with a call operator.
    template <typename C>
    basic_callable_ref(construct_from_t, C& obj) noexcept
        requires(not std::is_same_v<C, basic_callable_ref>)
      : _data{static_cast<void*>(&obj)}
      , _func{reinterpret_cast<_func_t>(&_cls_fn_call_op<C>)} {}

    /// @brief Construction a const reference to object with a call operator.
    template <typename C>
    basic_callable_ref(construct_from_t, const C& obj) noexcept
        requires(not std::is_same_v<C, basic_callable_ref>)
      : _data{static_cast<void*>(const_cast<C*>(&obj))}
      , _func{reinterpret_cast<_func_t>(&_cls_fn_call_op_c<C>)} {}

    /// @brief Construction a reference to object and member function constant.
    template <typename PtrT, PtrT ptr>
        requires(member_function_constant<PtrT, ptr>::
                   template invocable_with<RV, P...>)
    basic_callable_ref(
      optional_reference<typename member_function_constant<PtrT, ptr>::callee>
        obj,
      member_function_constant<PtrT, ptr> mfc) noexcept
      : _data{static_cast<void*>(
          const_cast<typename member_function_constant<PtrT, ptr>::scope*>(
            obj.get()))}
      , _func{reinterpret_cast<_func_t>(mfc.make_free())} {
        assert(_data != nullptr);
        assert(_func != nullptr);
    }

    /// @brief Indicates if this object stores a valid callable reference.
    [[nodiscard]] constexpr auto is_valid() const noexcept {
        return _func != nullptr;
    }

    /// @brief Indicates if this object stores a valid callable reference.
    /// @see is_valid
    [[nodiscard]] explicit constexpr operator bool() const noexcept {
        return is_valid();
    }

    /// @brief Call operator.
    /// @pre is_valid()
    template <typename... A>
    auto operator()(A&&... a) const -> RV {
        assert(is_valid());
        if(_data == nullptr) {
            return (reinterpret_cast<_func_pt>(_func))(std::forward<A>(a)...);
        } else {
            return (reinterpret_cast<_func_vpt>(_func))(
              _data, std::forward<A>(a)...);
        }
    }

    auto operator[](P... p) const -> RV {
        assert(is_valid());
        if(_data == nullptr) {
            return (reinterpret_cast<_func_pt>(_func))(std::move(p)...);
        } else {
            return (reinterpret_cast<_func_vpt>(_func))(_data, std::move(p)...);
        }
    }

private:
    void* _data{nullptr};
    void (*_func)() noexcept(NE){nullptr};

    using _func_t = void (*)() noexcept(NE);
    using _func_pt = RV (*)(P...) noexcept(NE);
    using _func_vpt = RV (*)(void*, P...) noexcept(NE);

    template <typename C>
    static auto _cls_fn_call_op(void* that, P... p) noexcept(NE) -> RV {
        assert(that);
        C& obj = *(static_cast<C*>(that));

        if constexpr(std::is_void_v<RV>) {
            obj(std::forward<P>(p)...);
        } else {
            return obj(std::forward<P>(p)...);
        }
    }

    template <typename C>
    static auto _cls_fn_call_op_c(void* that, P... p) noexcept(NE) -> RV {
        assert(that);
        const C& obj = *(static_cast<const C*>(that));

        if constexpr(std::is_void_v<RV>) {
            obj(std::forward<P>(p)...);
        } else {
            return obj(std::forward<P>(p)...);
        }
    }
};

/// @brief Alias for callable object references.
/// @ingroup functional
export template <typename Sig>
using callable_ref = basic_callable_ref<Sig, is_noexcept_function_v<Sig>>;
//------------------------------------------------------------------------------
export template <typename Sig, auto Ptr>
struct member_function_callable_ref : callable_ref<Sig> {
    constexpr member_function_callable_ref(auto* obj) noexcept
      : callable_ref<Sig>{obj, member_function_constant_t<Ptr>{}} {}
};
//------------------------------------------------------------------------------
template <
  typename RV,
  typename C,
  typename... P,
  bool NE,
  RV (C::*Ptr)(P...) noexcept(NE)>
[[nodiscard]] constexpr auto _make_callable_ref(
  optional_reference<C> obj,
  member_function_constant<RV (C::*)(P...) noexcept(NE), Ptr> mfc) noexcept
  -> callable_ref<RV(P...) noexcept(NE)> {
    return {obj, mfc};
}

/// @brief Function constructing instances of callable_ref wrapping member functions.
/// @ingroup functonal
export template <auto MemFuncPtr>
    requires(std::is_member_function_pointer_v<decltype(MemFuncPtr)>)
[[nodiscard]] constexpr auto make_callable_ref(
  optional_reference<typename member_function_constant_t<MemFuncPtr>::callee>
    obj) noexcept {
    return _make_callable_ref(obj, member_function_constant_t<MemFuncPtr>{});
}

export template <typename RV, typename C, typename... P, bool NE>
[[nodiscard]] auto make_callable_ref(
  optional_reference<C> obj,
  RV (*ptr)(C*, P...) noexcept(NE)) noexcept
  -> basic_callable_ref<RV(P...) noexcept(NE), NE> {
    return {obj, ptr};
}
//------------------------------------------------------------------------------
} // namespace eagine
