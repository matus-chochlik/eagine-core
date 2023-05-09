/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.c_api:key_value_list;

import std;
import eagine.core.types;
import eagine.core.memory;

namespace eagine::c_api {

/// @brief A single key/value pair for a key/value list.
/// @ingroup type_utils
/// @see key_value_list
export template <typename Traits>
struct key_value_list_element {
    /// @brief Alias for the key type.
    using key_type = typename Traits::key_type;
    /// @brief Alias for the value type.
    using value_type = typename Traits::value_type;

    key_type _key;
    value_type _value;

    /// @brief Construction from the key and the value.
    constexpr key_value_list_element(
      const key_type key,
      const value_type value) noexcept
      : _key{key}
      , _value{value} {}
};

/// @brief Template for classes wrapping static key/value typically attribute lists.
/// @ingroup type_utils
/// @tparam Traits policy class customizing the instantiation.
export template <typename Traits>
class key_value_list {
public:
    using key_type = typename Traits::key_type;
    using conv_type = typename Traits::conv_type;
    using value_type = typename Traits::value_type;

    constexpr key_value_list() noexcept {
        _values.push_back(Traits::terminator());
    }

    explicit constexpr key_value_list(
      const key_value_list_element<Traits>& e) noexcept {
        _values.reserve(3);
        _values.push_back(static_cast<conv_type>(e._key));
        _values.push_back(e._value);
        _values.push_back(Traits::terminator());
    }

    explicit constexpr key_value_list(
      const key_value_list_element<Traits>& l,
      const key_value_list_element<Traits>& r) noexcept {
        _values.reserve(5);
        _values.push_back(static_cast<conv_type>(l._key));
        _values.push_back(l._value);
        _values.push_back(static_cast<conv_type>(r._key));
        _values.push_back(r._value);
        _values.push_back(Traits::terminator());
    }

    /// @brief Returns a pointer to the internal element array.
    /// @see size
    auto data() const noexcept -> const value_type* {
        return _values.data();
    }

    /// @brief Returns the size of the element array (including the terminating zero).
    /// @see data
    constexpr auto size() const noexcept -> span_size_t {
        return span_size(_values.size());
    }

    /// @brief Appends a key/value pair and returns a new extended list.
    constexpr auto append(const key_value_list_element<Traits>& key_val)
      const noexcept -> key_value_list<Traits> {
        return {*this, key_val};
    }

    constexpr auto get() const noexcept {
        return view(_values);
    }

    constexpr operator span<const value_type>() const noexcept {
        return view(_values);
    }

private:
    constexpr key_value_list(
      const key_value_list& base,
      const key_value_list_element<Traits>& key_val) noexcept {
        _values.reserve(safe_add(base._values.size(), 2));
        for(const auto i : integer_range(base._values.size() - 1)) {
            _values.push_back(base._values[i]);
        }
        _values.push_back(static_cast<conv_type>(key_val._key));
        _values.push_back(key_val._value);
        _values.push_back(Traits::terminator());
    }

    std::vector<value_type> _values{};
};

/// @brief Concatenates two individual key/value elements into a key/value list.
/// @ingroup type_utils
/// @relates key_value_list
/// @see key_value_list_element
export template <typename Traits>
constexpr auto operator+(
  const key_value_list_element<Traits>& l,
  const key_value_list_element<Traits>& r) noexcept -> key_value_list<Traits> {
    return key_value_list<Traits>(l, r);
}

/// @brief Adds a key/value pair into a key/value list, returns a new list.
/// @ingroup type_utils
/// @relates key_value_list
/// @see key_value_list_element
export template <typename Traits>
constexpr auto operator+(
  const key_value_list<Traits>& l,
  const key_value_list_element<Traits>& r) noexcept -> key_value_list<Traits> {
    return l.append(r);
}

} // namespace eagine::c_api
