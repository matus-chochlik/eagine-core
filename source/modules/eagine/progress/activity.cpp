/// @file
///
/// Copyright Matus Chochlik.
/// Distributed under the Boost Software License, Version 1.0.
/// See accompanying file LICENSE_1_0.txt or copy at
/// https://www.boost.org/LICENSE_1_0.txt
///
export module eagine.core.progress:activity;

import std;
import eagine.core.types;
import eagine.core.memory;
import eagine.core.runtime;
import :backend;

namespace eagine {
//------------------------------------------------------------------------------
/// @brief Class tracking the progress of a long-running activity.
/// @ingroup progress
/// @see puzzle_progress
export class activity_progress {
public:
    activity_progress(
      const activity_progress& parent,
      const string_view title,
      const span_size_t total_steps = 0) noexcept;

    activity_progress() noexcept = default;
    activity_progress(activity_progress&& temp) noexcept
      : _backend{std::move(temp._backend)}
      , _activity_id{std::exchange(temp._activity_id, 0U)} {}
    activity_progress(const activity_progress&) = delete;
    auto operator=(activity_progress&& temp) noexcept -> activity_progress& {
        using std::swap;
        swap(this->_backend, temp._backend);
        swap(this->_activity_id, temp._activity_id);
        return *this;
    }
    auto operator=(const activity_progress&) = delete;

    /// @brief Marks the activity as finished.
    /// @see finish
    ~activity_progress() noexcept;

    /// @brief Creates a sub-activity progress tracker with a title.
    auto activity(const string_view title, const span_size_t total_steps = 0)
      const noexcept -> activity_progress;

    /// @brief Returns the unique id of this logger instance.
    [[nodiscard]] auto activity_id() const noexcept -> activity_progress_id_t {
        return _activity_id;
    }

    auto configure(basic_config_intf& config) const -> bool;

    /// @brief Updates the activity progress to the specified current value.
    /// @see finish
    /// @see advance_progress
    ///
    /// The return value indicates if the activity was canceled, true values
    /// means that the activity should continue, false means that the activity
    /// was canceled and should discontinue.
    auto update_progress(span_size_t current) const noexcept -> bool;

    /// @brief Advances the activity progress by the specified increment.
    /// @see finish
    /// @see update_progress
    ///
    /// The return value indicates if the activity was canceled, true values
    /// means that the activity should continue, false means that the activity
    /// was canceled and should discontinue.
    auto advance_progress(span_size_t increment = 1) const noexcept -> bool;

    /// @brief Explicitly marks the activity as finished.
    /// @see advance_progress
    /// @see update_progress
    void finish() noexcept;

protected:
    activity_progress(shared_holder<progress_tracker_backend> backend) noexcept
      : _backend{std::move(backend)} {}

    auto backend() const noexcept
      -> optional_reference<progress_tracker_backend>;

private:
    shared_holder<progress_tracker_backend> _backend;
    activity_progress_id_t _activity_id{0U};
};
//------------------------------------------------------------------------------
/// @brief Class tracking the completion of Count objects, indicating true/false status.
/// @ingroup progress
/// @see activity_progress
export template <span_size_t Count>
class puzzle_progress : activity_progress {
public:
    /// @brief Construction from a parent activity and this activity title.
    puzzle_progress(
      const activity_progress& parent,
      const string_view title) noexcept
      : activity_progress{parent, title, Count} {}

    /// @brief Indicates that all the pieces indicates true state.
    auto done() const noexcept -> bool {
        return _all_done;
    }

    /// @brief Updates the progress value by counting pieces returning true.
    template <typename... Piece>
    auto update_progress(const Piece&... piece) noexcept -> bool {
        const bool result{
          activity_progress::update_progress((... + span_size_t(bool(piece))))};
        _all_done = (bool(piece) and ...);
        if(_all_done) {
            activity_progress::finish();
        }
        return result;
    }

private:
    bool _all_done{false};
};
//------------------------------------------------------------------------------
} // namespace eagine
