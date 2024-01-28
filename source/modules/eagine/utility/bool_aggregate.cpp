/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.utility:bool_aggregate;
import std;

namespace eagine {

/// @brief Class storing initially false value and logically or-ing other values.
/// @ingroup type_utils
export template <typename Bool>
class basic_some_true {
public:
    /// @brief Default constructible.
    constexpr basic_some_true() noexcept = default;
    /// @brief Initializing constructior.
    constexpr basic_some_true(const bool init) noexcept
      : _result{init} {}

    /// @brief Copy  constructior.
    template <typename B>
    constexpr basic_some_true(const basic_some_true<B>& init) noexcept
      : basic_some_true{bool(init)} {}

    /// @brief Logically or-s the stored state with the specified argument.
    constexpr auto operator()(const bool b = true) noexcept -> auto& {
        _result = b or _result;
        return *this;
    }

    /// @brief Logically or-s the stored state with the specified argument.
    template <typename B>
    constexpr auto operator()(const basic_some_true<B> that) noexcept -> auto& {
        _result = bool(that) or _result;
        return *this;
    }

    /// @brief Returns the current boolean state.
    [[nodiscard]] constexpr explicit operator bool() const noexcept {
        return bool(_result);
    }

private:
    Bool _result{false};
};

/// @brief Class storing initially false value and logically or-ing other values.
/// @ingroup type_utils
export using some_true = basic_some_true<bool>;

export using some_true_atomic = basic_some_true<std::atomic<bool>>;

/// @brief Type used as return value indicating that some work was done.
/// @ingroup type_utils
export class work_done : public some_true {
public:
    constexpr work_done() noexcept = default;
    constexpr work_done(bool value) noexcept
      : some_true{value} {}

    template <typename Bool>
    constexpr work_done(const basic_some_true<Bool>& that) noexcept
      : some_true{that} {}

    /// @brief Suspend this thread for specified interval if no work was done.
    template <typename R, typename P>
    auto or_sleep_for(std::chrono::duration<R, P> interval) noexcept -> auto& {
        if(not *this) {
            std::this_thread::sleep_for(interval);
        }
        return *this;
    }
};

} // namespace eagine
