/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.container:iterator;

import <compare>;
import <iterator>;
import <utility>;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Template used in implementation of some iterators.
/// @tparam T the wrapped iterable type.
/// @tparam Derived the type derived from this template.
/// @see basic_selfref_iterator
/// @see basic_transforming_iterator
/// @ingroup type_utils
export template <typename T, typename Derived>
class basic_iterable_type {
public:
    /// @brief Alias for the value type.
    using value_type = T;

    /// @brief Alias for the difference type.
    using difference_type = decltype(std::declval<T>() - std::declval<T>());

    /// @brief Default constructor.
    constexpr basic_iterable_type() = default;

    /// @brief Initializing constructor.
    constexpr basic_iterable_type(const T value) noexcept
      : _value{value} {}

    /// @brief Explicit conversion to the wrapped iterable type.
    explicit constexpr operator T() const noexcept {
        return _value;
    }

    /// @brief Pre-increment operator.
    constexpr auto operator++() noexcept -> auto& {
        ++_value;
        return self();
    }

    /// @brief Addition operator.
    constexpr auto operator+=(const difference_type d) noexcept -> auto& {
        _value += d;
        return self();
    }

    /// @brief Post-increment operator.
    constexpr auto operator++(int) noexcept -> Derived {
        Derived res(self());
        ++_value;
        return res;
    }

    /// @brief Addition operator.
    friend constexpr auto operator+(
      const basic_iterable_type a,
      const difference_type d) noexcept {
        Derived res(a.self());
        res += d;
        return res;
    }

    /// @brief Pre-decrement operator.
    auto constexpr operator--() noexcept -> auto& {
        --_value;
        return self();
    }

    /// @brief Subtraction operator.
    auto constexpr operator-=(const difference_type d) noexcept -> auto& {
        _value -= d;
        return self();
    }

    /// @brief Post-decrement operator.
    auto constexpr operator--(int) noexcept -> Derived {
        Derived res(self());
        --_value;
        return res;
    }

    /// @brief Difference operator.
    friend constexpr auto operator-(
      const basic_iterable_type a,
      const difference_type d) noexcept {
        Derived res(a.self());
        res -= d;
        return res;
    }

    /// @brief Subtraction operator.
    constexpr auto operator-(const basic_iterable_type that) noexcept
      -> difference_type {
        return _value - that._value;
    }

    /// @brief Comparison.
    [[nodiscard]] constexpr auto operator<=>(
      const basic_iterable_type) const noexcept = default;

protected:
    T _value{};

    constexpr auto self() noexcept -> Derived& {
        return *static_cast<Derived*>(this);
    }
};
//------------------------------------------------------------------------------
/// @brief Iterator referencing the wrapped iterable type.
/// @tparam T the wrapped iterable type.
/// @tparam Derived the type derived from this template.
/// @ingroup type_utils
/// @see selfref_iterator
export template <typename T, typename Derived>
class basic_selfref_iterator : public basic_iterable_type<T, Derived> {
public:
    /// @brief Alias for reference type.
    using reference = const T&;

    /// @brief Alias for pointer type.
    using pointer = const T*;

    /// @brief Iterator caregoty.
    using iterator_category = std::random_access_iterator_tag;

    using basic_iterable_type<T, Derived>::basic_iterable_type;

    /// @brief Dereference operator.
    [[nodiscard]] constexpr auto operator*() const noexcept -> const T& {
        return this->_value;
    }
};
//------------------------------------------------------------------------------
/// @brief Default instantiation of basic_selfref_iterator.
/// @tparam T the wrapped iterable and value type.
/// @ingroup type_utils
export template <typename T>
struct selfref_iterator : basic_selfref_iterator<T, selfref_iterator<T>> {
    using basic_selfref_iterator<T, selfref_iterator<T>>::basic_selfref_iterator;
};
//------------------------------------------------------------------------------
/// @brief Iterator transforming the referenced elements.
/// @tparam Iterator the underlying iterator type.
/// @tparam T the transformed value type of this iterator.
/// @tparam S the value type of the underlying iterator.
/// @tparam Transform type of the transfomation callable.
/// @tparam Derived the type derived from this template.
/// @see transforming_iterator
/// @ingroup type_utils
export template <
  typename Iterator,
  typename T,
  typename S,
  typename Transform,
  typename Derived>
class basic_transforming_iterator
  : public basic_selfref_iterator<Iterator, Derived> {
public:
    /// @brief Default constructor.
    constexpr basic_transforming_iterator() = default;

    /// @brief Initialization for the underlying iterator and transform callable.
    constexpr basic_transforming_iterator(const Iterator iter, Transform transf)
      : _base{iter}
      , _transf{std::move(transf)} {}

    /// @brief Alias for value type.
    using value_type = T;

    /// @brief Alias for pointer type.
    using reference = const T&;

    /// @brief Alias for pointer type.
    using pointer = const T*;

    /// @brief Dereference operation.
    [[nodiscard]] constexpr auto operator*() const -> const T& {
        _tempval = _transf(**_base_iter());
        return _tempval;
    }

private:
    using _base = basic_selfref_iterator<Iterator, Derived>;
    Transform _transf{};

    mutable S _tempval{};

    constexpr auto _base_iter() const noexcept -> const _base& {
        return *this;
    }
};
//------------------------------------------------------------------------------
export template <typename Iterator, typename T, typename Transform, typename Derived>
class basic_transforming_iterator<Iterator, T, T&, Transform, Derived>
  : public basic_selfref_iterator<Iterator, Derived> {
    using _base = basic_selfref_iterator<Iterator, Derived>;

public:
    constexpr basic_transforming_iterator() = default;

    constexpr basic_transforming_iterator(Iterator iter, Transform transf)
      : _base{iter}
      , _transf{std::move(transf)} {}

    using value_type = T;
    using reference = const T&;
    using pointer = const T*;

    [[nodiscard]] constexpr auto operator*() -> T& {
        return _transf(**_base_iter());
    }

    [[nodiscard]] constexpr auto operator*() const -> const T& {
        return _transf(**_base_iter());
    }

private:
    Transform _transf{};

    auto _base_iter() const noexcept -> const _base& {
        return *this;
    }
};
//------------------------------------------------------------------------------
/// @brief Default instantiation of basic_transforming_iterator
/// @tparam Iterator the underlying iterator type.
/// @tparam T the transformed value type of this iterator.
/// @tparam S the value type of the underlying iterator.
/// @tparam Transform type of the transfomation callable.
/// @ingroup type_utils
export template <typename Iterator, typename T, typename S, typename Transform>
struct transforming_iterator
  : basic_transforming_iterator<
      Iterator,
      T,
      S,
      Transform,
      transforming_iterator<Iterator, T, S, Transform>> {
    using basic_transforming_iterator<
      Iterator,
      T,
      S,
      Transform,
      transforming_iterator<Iterator, T, S, Transform>>::
      basic_transforming_iterator;
};
//------------------------------------------------------------------------------
/// @brief Iterator type-casting the referenced elements.
/// @tparam Iterator the underlying iterator type.
/// @tparam T the type-case value type of this iterator.
/// @tparam S the value type of the underlying iterator.
/// @tparam Derived the type derived from this template.
/// @see transforming_iterator
/// @ingroup type_utils
export template <typename Iterator, typename T, typename S, typename Derived>
class basic_noexcept_casting_iterator
  : public basic_transforming_iterator<
      Iterator,
      T,
      S,
      S (*)(typename std::iterator_traits<Iterator>::reference) noexcept,
      Derived> {
    using _base = basic_transforming_iterator<
      Iterator,
      T,
      S,
      S (*)(typename std::iterator_traits<Iterator>::reference) noexcept,
      Derived>;

public:
    constexpr basic_noexcept_casting_iterator(const Iterator iter)
      : _base{iter, &_cast} {}

private:
    static constexpr auto _cast(
      typename std::iterator_traits<Iterator>::reference r) noexcept -> T {
        return static_cast<T>(r);
    }
};
//------------------------------------------------------------------------------
/// @brief Default instantiation of basic_noexcept_casting_iterator.
/// @tparam Iterator the underlying iterator type.
/// @tparam T the type-case value type of this iterator.
/// @tparam S the value type of the underlying iterator.
/// @ingroup type_utils
export template <typename Iterator, typename T, typename S>
struct noexcept_casting_iterator
  : basic_noexcept_casting_iterator<
      Iterator,
      T,
      S,
      noexcept_casting_iterator<Iterator, T, S>> {
    using basic_noexcept_casting_iterator<
      Iterator,
      T,
      S,
      noexcept_casting_iterator<Iterator, T, S>>::basic_noexcept_casting_iterator;
};
//------------------------------------------------------------------------------
/// @brief Adapter reversing the iteration of elements in a Range.
/// @ingroup type_utils
export template <typename Range>
class reverse {
public:
    /// @brief Construction from a reference to a range.
    constexpr reverse(Range& range) noexcept
      : _range{range} {}

    /// @brief Returns a reverse iterator to the last element in the adapted range.
    [[nodiscard]] constexpr auto begin() noexcept {
        return _range.rbegin();
    }

    /// @brief Returns a reverse iterator past the first element in the adapted range.
    [[nodiscard]] constexpr auto end() noexcept {
        return _range.rend();
    }

    /// @brief Returns a reverse iterator to the last element in the adapted range.
    [[nodiscard]] constexpr auto begin() const noexcept {
        return _range.rbegin();
    }

    /// @brief Returns a reverse iterator past the first element in the adapted range.
    [[nodiscard]] constexpr auto end() const noexcept {
        return _range.rend();
    }

private:
    Range& _range;
};
//------------------------------------------------------------------------------
} // namespace eagine
