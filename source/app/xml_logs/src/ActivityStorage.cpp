///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#include "ActivityStorage.hpp"
#include "EntryStorage.hpp"
#include <algorithm>
//------------------------------------------------------------------------------
void ActivityStorage::beginStream(std::uintptr_t stream_id) noexcept {
    EAGINE_MAYBE_UNUSED(stream_id);
}
//------------------------------------------------------------------------------
void ActivityStorage::endStream(std::uintptr_t stream_id) noexcept {
    _activities.erase(
      std::remove_if(
        _activities.begin(),
        _activities.end(),
        [stream_id](const auto& activity) -> bool {
            return activity.stream_id == stream_id;
        }),
      _activities.end());
}
//------------------------------------------------------------------------------
void ActivityStorage::addEntry(LogEntryData& entry) noexcept {
    EAGINE_MAYBE_UNUSED(entry);
}
//------------------------------------------------------------------------------
