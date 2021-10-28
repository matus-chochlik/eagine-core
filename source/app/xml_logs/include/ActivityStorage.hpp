///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#ifndef EAGINE_XML_LOGS_ACTIVITY_STORAGE_HPP
#define EAGINE_XML_LOGS_ACTIVITY_STORAGE_HPP

#include <eagine/identifier.hpp>
#include <eagine/logging/fwd.hpp>
#include <eagine/logging/severity.hpp>
#include <cstdint>
#include <vector>

class Backend;
struct LogEntryData;
//------------------------------------------------------------------------------
struct ActivityData {
    std::uintptr_t stream_id;
    std::uint64_t instance;
    eagine::identifier source;
    eagine::identifier arg;
    float min{0.F};
    float value{0.F};
    float max{1.F};
    std::string message;
    eagine::log_event_severity severity;
};
//------------------------------------------------------------------------------
class ActivityStorage {
public:
    void beginStream(std::uintptr_t stream_id) noexcept;
    void endStream(std::uintptr_t stream_id) noexcept;
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

private:
    std::vector<ActivityData> _activities;
};
//------------------------------------------------------------------------------

#endif

