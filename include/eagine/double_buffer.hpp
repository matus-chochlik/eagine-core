/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///

#ifndef EAGINE_DOUBLE_BUFFER_HPP
#define EAGINE_DOUBLE_BUFFER_HPP

#include <array>

namespace eagine {

/// @brief Class holding two instances of type @p T and allows to switch them.
/// @ingroup type_utils
template <typename T>
class double_buffer {
public:
    /// @brief Default constructor.
    double_buffer() = default;

    /// @brief Constructor setting the initial value for both buffers.
    double_buffer(const T& initial)
      : _values{{initial, initial}} {}

    /// @brief Swaps the front and the back buffers.
    /// @see front
    /// @see back
    void swap() noexcept {
        _idx = !_idx;
    }

    /// @brief Returns a reference to the current "front" instance.
    auto front() noexcept -> T& {
        return _values[front_index()];
    }

    /// @brief Returns a const reference to the current "front" instance.
    auto front() const noexcept -> const T& {
        return _values[front_index()];
    }

    /// @brief Returns a reference to the current "back" instance.
    auto back() noexcept -> T& {
        return _values[back_index()];
    }

    /// @brief Returns a const reference to the current "back" instance.
    auto back() const noexcept -> const T& {
        return _values[back_index()];
    }

private:
    auto front_index() const noexcept -> std::size_t {
        return static_cast<std::size_t>(_idx);
    }

    auto back_index() const noexcept -> std::size_t {
        return static_cast<std::size_t>(!_idx);
    }

    std::array<T, 2> _values{};
    bool _idx{false};
};

} // namespace eagine

#endif // EAGINE_DOUBLE_BUFFER_HPP
