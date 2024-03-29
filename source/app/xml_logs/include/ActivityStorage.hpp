///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#ifndef EAGINE_XML_LOGS_ACTIVITY_STORAGE_HPP
#define EAGINE_XML_LOGS_ACTIVITY_STORAGE_HPP

import eagine.core;
import std;

class Backend;
struct LogEntryData;
struct LogStreamInfo;
using stream_id_t = std::uintptr_t;
//------------------------------------------------------------------------------
struct ActivityData {
    stream_id_t streamId;
    std::uint64_t instance;
    eagine::identifier source;
    eagine::identifier arg;
    float min{0.F};
    float value{0.F};
    float max{1.F};
    std::string message;
    eagine::log_event_severity severity;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point updateTime;
    std::size_t remainingUpdatePos{0U};
    std::array<float, 8> remainingTimes{};

    auto init(const LogEntryData&, const eagine::identifier) noexcept
      -> ActivityData&;
    auto update(
      const LogEntryData&,
      const std::tuple<float, float, float>&) noexcept -> ActivityData&;

    auto doneRatio() const noexcept -> float;
    auto todoRatio() const noexcept -> float;

    auto timeSinceStart() const noexcept -> std::chrono::duration<float>;
    auto timeSinceUpdate() const noexcept -> std::chrono::duration<float>;

    auto hasTimeEstimation() const noexcept -> bool;
    auto estimatedRemainingTime() const noexcept
      -> std::chrono::duration<float>;

    auto isDone() const noexcept -> bool;
};
//------------------------------------------------------------------------------
class ActivityStorage {
public:
    void beginStream(stream_id_t stream_id, const LogStreamInfo&) noexcept;
    void endStream(stream_id_t stream_id) noexcept;
    void addEntry(const LogEntryData& entry) noexcept;

    auto activityCount() const noexcept -> int {
        return eagine::limit_cast<int>(_activities.size());
    }

    auto getActivity(int index) noexcept -> ActivityData* {
        if(const auto actIdx{eagine::convert_if_fits<std::size_t>(index)}) {
            if(extract(actIdx) < _activities.size()) {
                return &_activities[extract(actIdx)];
            }
        }
        return nullptr;
    }

    auto cleanupDone() noexcept -> bool;

private:
    auto _getEntryActivity(
      const LogEntryData& entry,
      const eagine::identifier entryArg) noexcept -> ActivityData&;
    std::vector<ActivityData> _activities;
};
//------------------------------------------------------------------------------

#endif

