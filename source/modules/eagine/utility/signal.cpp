/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.utility:signal;

import eagine.core.types;
import eagine.core.container;
import :callable_ref;
import <limits>;
import <utility>;

namespace eagine {
//------------------------------------------------------------------------------
export using signal_binding_key = unsigned;

export template <typename Signature>
class signal;

export class signal_binding;
//------------------------------------------------------------------------------
/// @brief Class that allows connecting and invoking callables with the specified signature.
/// @ingroup msgbus
/// @see signal_binding
export template <typename... Params>
class signal<void(Params...) noexcept> {
public:
    /// @brief Type of key that can be used to remove connected slot.
    using binding_key = signal_binding_key;

    /// @brief Connects the specified callable, returns a key for removal.
    /// @see bind
    /// @see disconnect
    auto connect(const callable_ref<void(Params...) noexcept> slot)
      -> binding_key {
        const auto get_key = [](const auto& entry) {
            return std::get<0>(entry);
        };
        auto key = 1U;
        auto pos = _slots.begin();

        if(!_slots.empty()) {
            key = get_key(_slots.back());
            key = key != std::numeric_limits<signal_binding_key>::max()
                    ? key + 1
                    : 1U;
            pos = [&]() {
                while(true) {
                    auto [p, unused] = _slots.find_insert_position(key);
                    if(unused) {
                        return p;
                    }
                    ++key;
                }
            }();
        }
        assert(key);
        _slots.insert(pos, {key, slot});
        return key;
    }

    /// @brief Connects the specified callable, returns a binding object.
    /// @see connect
    /// @see signal_binding
    [[nodiscard]] auto bind(const callable_ref<void(Params...) noexcept> slot)
      -> signal_binding;

    /// @brief Disconnects the callable by specifying the connection key.
    /// @see connect
    auto disconnect(const binding_key key) noexcept -> auto& {
        _slots.erase(key);
        return *this;
    }

    void unbind(const binding_key key) noexcept {
        _slots.erase(key);
    }

    /// @brief The call operator.
    void operator()(Params... args) const noexcept {
        _call(args...);
    }

    /// @brief Implicit conversion to a compatible callable_ref.
    operator callable_ref<void(Params...) noexcept>() const noexcept {
        return {
          this,
          member_function_constant<
            void (signal::*)(Params...) const noexcept,
            &signal::_call>{}};
    }

    template <typename... P>
    operator callable_ref<void(P...) noexcept>() const noexcept {
        return {
          this,
          member_function_constant<
            void (signal::*)(P...) const noexcept,
            &signal::_call_tpl<P...>>{}};
    }

private:
    void _call(Params... args) const noexcept {
        for(const auto& entry : _slots) {
            std::get<1>(entry)(args...);
        }
    }

    template <typename... P>
    void _call_tpl(P... args) const noexcept {
        for(const auto& entry : _slots) {
            std::get<1>(entry)(args...);
        }
    }

    flat_map<unsigned, callable_ref<void(Params...) noexcept>> _slots;
};

export template <auto MemFuncPtr, typename C, typename... Params>
auto connect(C* that, signal<void(Params...) noexcept>& sig) noexcept {
    return sig.connect(make_callable_ref<MemFuncPtr>(that));
}
//------------------------------------------------------------------------------
/// @brief Class that keeps a signam binding key and disconnects it when destroyed.
/// @ingroup msgbus
/// @see signal
export class signal_binding {
public:
    /// @brief Default constructor.
    constexpr signal_binding() noexcept = default;

    template <typename Signature>
    signal_binding(const signal_binding_key key, signal<Signature>* sig) noexcept
      : _key{key}
      , _disconnect{
          sig,
          member_function_constant_t<&signal<Signature>::unbind>{}} {}

    /// @brief Move constructible.
    signal_binding(signal_binding&& temp) noexcept
      : _key{std::exchange(temp._key, 0U)}
      , _disconnect{std::move(temp._disconnect)} {}

    /// @brief Not copy constructible.
    signal_binding(const signal_binding&) = delete;

    /// 2Brief Disconnects the associated binding.
    ~signal_binding() noexcept {
        if(_key && _disconnect) {
            _disconnect(_key);
        }
    }

    /// @brief Move assignable.
    auto operator=(signal_binding&& temp) noexcept -> signal_binding& {
        using std::swap;
        swap(this->_key, temp._key);
        swap(this->_disconnect, temp._disconnect);
        return *this;
    }

    /// @brief Not copy assignable.
    auto operator=(const signal_binding&) = delete;

    /// @brief Indicates if this binding is connected.
    constexpr auto is_connected() const noexcept -> bool {
        return _key != 0;
    }

    /// @brief Indicates if this binding is connected.
    /// @see is_connected
    constexpr operator bool() const noexcept {
        return is_connected();
    }

    /// @brief Disconnects this binding from it's signal.
    void disconnect() noexcept {
        if(_key && _disconnect) {
            _disconnect(_key);
            _key = 0U;
        }
    }

private:
    signal_binding_key _key{0U};
    callable_ref<void(signal_binding_key) noexcept> _disconnect;
};
//------------------------------------------------------------------------------
template <typename... Params>
[[nodiscard]] auto signal<void(Params...) noexcept>::bind(
  callable_ref<void(Params...) noexcept> slot) -> signal_binding {
    return {connect(slot), this};
}

export template <auto MemFuncPtr, typename C, typename... Params>
auto bind(C* that, signal<void(Params...) noexcept>& sig) noexcept {
    return sig.bind(make_callable_ref<MemFuncPtr>(that));
}
//------------------------------------------------------------------------------
} // namespace eagine

