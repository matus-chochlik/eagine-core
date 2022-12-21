/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.utility:double_buffer;

import <array>;
import <cstdint>;

namespace eagine {

/// @brief Class holding two instances of type @p T and allows to switch them.
/// @ingroup type_utils
export template <typename T>
class double_buffer {
public:
    /// @brief Default constructor.
    double_buffer() = default;

    /// @brief Constructor setting the initial value for both buffers.
    double_buffer(const T& initial)
      : _values{{initial, initial}} {}

    double_buffer(double_buffer&&) noexcept = default;
    double_buffer(const double_buffer&) = default;
    auto operator=(double_buffer&&) noexcept -> double_buffer& = default;
    auto operator=(const double_buffer&) -> double_buffer& = default;
    ~double_buffer() noexcept = default;

    /// @brief Swaps the front and the back buffers.
    /// @see front
    /// @see back
    void swap() noexcept {
        _idx = not _idx;
    }

    /// @brief Returns a reference to the current "front" instance.
    [[nodiscard]] auto front() noexcept -> T& {
        return _values[front_index()];
    }

    /// @brief Returns a const reference to the current "front" instance.
    [[nodiscard]] auto front() const noexcept -> const T& {
        return _values[front_index()];
    }

    /// @brief Returns a reference to the current "back" instance.
    [[nodiscard]] auto back() noexcept -> T& {
        return _values[back_index()];
    }

    /// @brief Returns a const reference to the current "back" instance.
    [[nodiscard]] auto back() const noexcept -> const T& {
        return _values[back_index()];
    }

private:
    auto front_index() const noexcept -> std::size_t {
        return static_cast<std::size_t>(_idx);
    }

    auto back_index() const noexcept -> std::size_t {
        return static_cast<std::size_t>(not _idx);
    }

    std::array<T, 2> _values{};
    bool _idx{false};
};

} // namespace eagine
