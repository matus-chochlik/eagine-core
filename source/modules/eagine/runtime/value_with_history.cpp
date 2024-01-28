/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
module;

#include <cassert>

export module eagine.core.runtime:value_with_history;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.valid_if;

namespace eagine {
//------------------------------------------------------------------------------
export template <typename T>
constexpr auto value_with_history_changed(const T& a, const T& b) noexcept
  -> bool {
    return not are_equal(a, b);
}

export template <typename T>
constexpr auto value_with_history_delta(
  const T& new_value,
  const T& old_value) noexcept {
    return new_value - old_value;
}

export constexpr auto value_with_history_delta(
  bool new_value,
  bool old_value) noexcept -> int {
    return int(new_value) - int(old_value);
}

export template <typename T>
constexpr auto value_with_history_distance(
  const T& new_value,
  const T& old_value) noexcept {
    using std::abs;
    return abs(value_with_history_delta(new_value, old_value));
}
//------------------------------------------------------------------------------
/// @brief Base storage class for values and variables with history.
/// @tparam T the type of the value
/// @tparam N the number of latest historical revisions of the stored value.
/// @ingroup value_history
/// @see value_with_history
/// @note Do not use this class directly.
///       Use value_with_history or variable_with_history.
export template <typename T, std::size_t N>
class value_with_history_storage {
public:
    /// @brief Default constructor.
    constexpr value_with_history_storage() = default;

    /// @brief Initializes the individual revisions of the value.
    template <typename... I>
    constexpr value_with_history_storage(I&&... initial)
        requires(sizeof...(I) == N)
      : _values{T(initial)...} {}

    /// @brief Initializes all revisions with the same initial value.
    constexpr value_with_history_storage(const T& initial) noexcept {
        std::fill(_values.begin(), _values.end(), initial);
    }

    /// @brief Returns a view of the elements (from the oldest to the newest).
    constexpr auto view() const noexcept {
        return memory::view(_values);
    }

    /// @brief Returns an iterator to the first (oldest) value.
    auto begin() const noexcept {
        return _values.begin();
    }

    /// @brief Returns an iterator past the last (newest) value.
    auto end() const noexcept {
        return _values.end();
    }

    /// @brief Returns the i-th revision of the stored value (0 = current value).
    constexpr auto get(std::size_t i) const noexcept -> const T& {
        assert(i < N);
        return _values[N - 1 - i];
    }

    /// @brief Returns the newest revision of the stored value.
    constexpr auto get() const noexcept -> const T& {
        return _values.back();
    }

    /// @brief Sets the current revision of the stored value.
    constexpr void set(std::size_t i, T value) noexcept {
        assert(i < N);
        _values[N - 1 - i] = std::move(value);
    }

    /// @brief Sets the current revision of the stored value.
    constexpr void set(T value) noexcept {
        _values.back() = std::move(value);
    }

    /// @brief Move the stored revisions by one, to make place for new value.
    /// @see sync
    constexpr void shift() noexcept {
        std::rotate(_values.begin(), _values.begin() + 1U, _values.end());
    }

    /// @brief Synchronize the historic revisions to the current value.
    /// @see shift
    constexpr void sync() noexcept {
        std::fill(_values.begin(), _values.end() - 1U, get());
    }

private:
    std::array<T, N> _values{};
};
//------------------------------------------------------------------------------
export template <typename Transform, typename... T, std::size_t N>
auto transform_stored_values(
  Transform transform_op,
  const value_with_history_storage<T, N>&... v) {
    value_with_history_storage<
      decltype(std::declval<Transform>()(std::declval<T>()...)),
      N>
      result;

    for(const auto i : integer_range(N)) {
        result.set(i, transform_op(v.get(i)...));
    }
    return result;
}
//------------------------------------------------------------------------------
export template <typename Delta, typename T, std::size_t N>
constexpr auto differentiate_stored_values(
  Delta delta_op,
  const value_with_history_storage<T, N>& v) {
    value_with_history_storage<std::invoke_result_t<Delta, T, T>, N - 1> result;

    for(const auto i : integer_range(1U, N)) {
        result.set(i - 1, delta_op(v.get(i - 1), v.get(i)));
    }
    return result;
}
//------------------------------------------------------------------------------
export template <typename U, typename T, std::size_t N>
auto convert_stored_values(const value_with_history_storage<T, N>& storage) {
    return transform_stored_values([](const T& v) { return U(v); }, storage);
}
//------------------------------------------------------------------------------
/// @brief Class for read-only values with history.
/// @tparam T the type of the value
/// @tparam N the number of latest historical revisions of the stored value.
/// @ingroup value_history
export template <typename T, std::size_t N>
class value_with_history {
public:
    explicit value_with_history(const value_with_history_storage<T, N>& storage)
      : _values{storage} {}

    /// @brief Initialized the individual revisions of this value.
    template <typename... I>
    constexpr value_with_history(I&&... initial)
      : _values{std::forward<I>(initial)...} {}

    /// @brief Returns a reference to the value storage.
    constexpr auto values() const noexcept
      -> const value_with_history_storage<T, N>& {
        return _values;
    }

    /// @brief Returns a view of the elements (from the oldest to the newest).
    constexpr auto view_history() const noexcept {
        return values().view();
    }

    /// @brief Returns the number of elements.
    constexpr auto size() const noexcept {
        return span_size(N);
    }

    /// @brief Returns an iterator to the oldest value.
    constexpr auto begin() const noexcept {
        return values().begin();
    }

    /// @brief Returns an iterator past the newest value.
    constexpr auto end() const noexcept {
        return values().end();
    }

    /// @brief Returns a reverse iterator to the newest value.
    constexpr auto rbegin() const noexcept {
        return std::reverse_iterator{end()};
    }

    /// @brief Returns a reverse iterator past value.
    constexpr auto rend() const noexcept {
        return std::reverse_iterator{begin()};
    }

    /// @brief Returns the current revision of the value.
    /// @see value
    constexpr auto get() const noexcept {
        return values().get();
    }

    /// @brief Returns the current revision of the value.
    /// @see get
    /// @see old_value
    constexpr auto value() const noexcept {
        return values().get();
    }

    /// @brief Returns the previous revision of the value.
    /// @see get
    /// @see value
    constexpr auto old_value() const noexcept {
        return values().get(1);
    }

    /// @brief Returns the current or previous revision of the value.
    /// @see value
    /// @see old_value
    constexpr auto value(bool old) const noexcept {
        return old ? old_value() : value();
    }

    /// @brief Returns the current revision of the value.
    /// @see value
    constexpr operator T() const noexcept {
        return value();
    }

    template <typename U, typename Po, typename L>
    constexpr operator basic_valid_if<U, Po, L>() const noexcept {
        return {U(value())};
    }

    /// @brief Returns the difference between the current and the previous revision.
    constexpr auto delta() const noexcept {
        return value_with_history_delta(value(), old_value());
    }

    /// @brief Returns the differences between the adjacent revisions.
    constexpr auto deltas() const noexcept {
        return value_with_history<decltype(delta()), N - 1>(
          differentiate_stored_values(
            [](const T& n, const T& o) {
                return value_with_history_delta(n, o);
            },
            values()));
    }

    /// @brief Returns the distance between the current and the previous revision.
    constexpr auto distance() const noexcept {
        return value_with_history_distance(value(), old_value());
    }

    /// @brief Indicates if the current and previous revisions differ.
    constexpr auto changed() const noexcept -> bool {
        return value_with_history_changed(old_value(), value());
    }

    /// @brief Returns this with the values cast to new type @p U.
    template <typename U>
    auto as() const {
        return value_with_history<U, N>(convert_stored_values<U>(values()));
    }

    /// @brief Synchronize the historic revisions to the current value.
    /// @see shift
    auto sync() noexcept -> auto& {
        values().sync();
        return *this;
    }

protected:
    auto values() noexcept -> value_with_history_storage<T, N>& {
        return _values;
    }

    void _set_value(const T& new_value) noexcept {
        values().shift();
        values().set(new_value);
    }

    auto _update_value(const T& new_value) noexcept -> bool {
        if(value_with_history_changed(values().get(), new_value)) {
            _set_value(new_value);
            return true;
        }
        return false;
    }

    auto _advance_value(const T& delta_value) noexcept -> bool {
        values().shift();
        values().set(values().get() + delta_value);
        return true;
    }

    value_with_history() = default;

    explicit value_with_history(const T& initial) noexcept
      : _values(initial) {}

private:
    static_assert(N >= 2, "at least two values are required");
    value_with_history_storage<T, N> _values;
};
//------------------------------------------------------------------------------
export template <typename Transform, typename... T, std::size_t N>
auto transform(Transform transform_op, const value_with_history<T, N>&... v) {
    return value_with_history<
      decltype(std::declval<Transform>()(std::declval<T>()...)),
      N>(transform_stored_values(transform_op, v.values()...));
}
//------------------------------------------------------------------------------
export template <typename T1, typename T2, std::size_t N>
auto operator*(
  const value_with_history<T1, N>& v1,
  const value_with_history<T2, N>& v2) noexcept {
    return transform(
      [](const T1& t1, const T2& t2) { return t1 * t2; }, v1, v2);
}
//------------------------------------------------------------------------------
export template <typename T1, typename T2, std::size_t N>
auto operator/(
  const value_with_history<T1, N>& v1,
  const value_with_history<T2, N>& v2) noexcept {
    return transform(
      [](const T1& t1, const T2& t2) { return t1 / t2; }, v1, v2);
}
//------------------------------------------------------------------------------
/// @brief Class for mutable variables with history.
/// @tparam T the type of the value
/// @tparam N the number of latest historical revisions of the stored value.
/// @ingroup value_history
export template <typename T, std::size_t N = 2>
class variable_with_history : public value_with_history<T, N> {
public:
    /// @brief Default constructor.
    constexpr variable_with_history() noexcept = default;

    /// @brief Initialize the all revisions to the initial value.
    constexpr variable_with_history(const T& initial) noexcept
      : value_with_history<T, N>(initial) {}

    /// @brief Returns this as a const reference to the base value with history.
    constexpr auto as_value() const noexcept
      -> const value_with_history<T, N>& {
        return *this;
    }

    /// @brief If new_value is different than current value shifts and assigns it.
    constexpr auto assign(const T& new_value) -> bool {
        return this->_update_value(new_value);
    }

    /// @brief If new_value is different than current value shifts and assigns it.
    /// @see operator<<
    constexpr auto operator=(const T& new_value) -> auto& {
        assign(new_value);
        return *this;
    }

    /// @brief Shifts the revisions and assigns a new value.
    /// @see operator=
    constexpr auto operator<<(const T& new_value) -> auto& {
        this->_set_value(new_value);
        return *this;
    }

    /// @brief Shifts the revisions and advanced the current value by given delta.
    constexpr auto advance(const T& delta_value) -> bool {
        return this->_advance_value(delta_value);
    }
};
//------------------------------------------------------------------------------
export template <typename T, typename Po, typename L, std::size_t N>
class variable_with_history<basic_valid_if<T, Po, L>, N>
  : public value_with_history<T, N> {
public:
    constexpr variable_with_history(
      const basic_valid_if<T, Po, L>& initial) noexcept
      : value_with_history<T, N>(initial.value()) {}

    constexpr auto assign(const basic_valid_if<T, Po, L>& new_value) -> bool {
        return this->_update_value(new_value.value());
    }

    constexpr auto operator=(const T& new_value) -> auto& {
        assign(new_value);
        return *this;
    }

    constexpr auto advance(const basic_valid_if<T, Po, L>& delta_value)
      -> bool {
        return this->_advance_value(delta_value.value());
    }
};
//------------------------------------------------------------------------------
} // namespace eagine

