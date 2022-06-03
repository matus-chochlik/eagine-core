/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.types:enum_map;

import :basic;
import :bitfield;
import :selector;
import <type_traits>;

namespace eagine {
//------------------------------------------------------------------------------
template <typename Enum, template <Enum> class Unit, Enum Key>
struct static_enum_map_unit : Unit<Key> {

    template <typename Visitor>
    auto _accept(const Enum key, Visitor& visitor) -> bool {
        if(Key == key) {
            visitor(Key, *static_cast<Unit<Key>*>(this));
            return true;
        }
        return false;
    }

    template <typename Visitor>
    auto _accept(const Enum key, Visitor& visitor) const -> bool {
        if(Key == key) {
            visitor(Key, *static_cast<const Unit<Key>*>(this));
            return true;
        }
        return false;
    }

    template <typename Visitor>
    auto _accept(const bitfield<Enum> keys, Visitor& visitor) -> span_size_t {
        if(keys.has(Key)) {
            visitor(Key, *static_cast<Unit<Key>*>(this));
            return 1;
        }
        return 0;
    }

    template <typename Visitor>
    auto _accept(const bitfield<Enum> keys, Visitor& visitor) const
      -> span_size_t {
        if(keys.has(Key)) {
            visitor(Key, *static_cast<const Unit<Key>*>(this));
            return 1;
        }
        return 0;
    }

    template <typename Visitor>
    auto _accept(Visitor& visitor) -> bool {
        visitor(Key, *static_cast<Unit<Key>*>(this));
        return true;
    }

    template <typename Visitor>
    auto _accept(Visitor& visitor) const -> bool {
        visitor(Key, *static_cast<const Unit<Key>*>(this));
        return true;
    }
};
//------------------------------------------------------------------------------
/// @brief Class mapping from an enumerator of an instantiation of a template.
/// @ingroup type_utils
export template <typename Enum, template <Enum> class Unit, Enum... Keys>
class static_enum_map : private static_enum_map_unit<Enum, Unit, Keys>... {

public:
    /// @brief Default constructor.
    static_enum_map() = default;

    /// @brief Explicit construction from arguments that are passed to the units.
    template <typename... Args>
    explicit static_enum_map(const construct_from_t, const Args&... args)
        requires(sizeof...(Args) > 1)
    : static_enum_map_unit<Enum, Unit, Keys>{args...}... {}

    /// @brief Returns a reference to the unit with the specified enumerator key.
    /// @see visit
    template <Enum Key>
    auto get() noexcept -> Unit<Key>& {
        return _base<Key>();
    }

    /// @brief Returns a const reference to the unit with the specified enumerator key.
    /// @see visit
    template <Enum Key>
    auto get() const noexcept -> const Unit<Key>& {
        return _base<Key>();
    }

    /// @brief Calls a visitor function on the unit with the specified key.
    /// @see visit_all
    /// @see get
    template <typename Visitor>
    auto visit(const Enum key, Visitor visitor) noexcept -> bool {
        return (false || ... || _base<Keys>()._accept(key, visitor));
    }

    /// @brief Calls a visitor function on the unit with the specified key.
    /// @see visit_all
    /// @see get
    template <typename Visitor>
    auto visit(const Enum key, Visitor visitor) const noexcept -> bool {
        return (false || ... || _base<Keys>()._accept(key, visitor));
    }

    /// @brief Calls a visitor function on the unit with the specified keys.
    /// @see visit_all
    /// @see get
    template <typename Visitor>
    auto visit(const bitfield<Enum> keys, Visitor visitor) noexcept
      -> span_size_t {
        return (0 + ... + _base<Keys>()._accept(keys, visitor));
    }

    /// @brief Calls a visitor function on the unit with the specified keys.
    /// @see visit_all
    /// @see get
    template <typename Visitor>
    auto visit(const bitfield<Enum> keys, Visitor visitor) const noexcept
      -> span_size_t {
        return (0 + ... + _base<Keys>()._accept(keys, visitor));
    }

    /// @brief Calls a visitor function on all units.
    /// @see visit
    template <typename Visitor>
    auto visit_all(Visitor visitor) noexcept -> bool {
        return (true && ... && _base<Keys>()._accept(visitor));
    }

    /// @brief Calls a visitor function on all units.
    /// @see visit
    template <typename Visitor>
    auto visit_all(Visitor visitor) const noexcept -> bool {
        return (true && ... && _base<Keys>()._accept(visitor));
    }

private:
    template <Enum Key>
    auto _base() noexcept -> static_enum_map_unit<Enum, Unit, Key>& {
        return *this;
    }

    template <Enum Key>
    auto _base() const noexcept
      -> const static_enum_map_unit<Enum, Unit, Key>& {
        return *this;
    }
};
//------------------------------------------------------------------------------
} // namespace eagine
