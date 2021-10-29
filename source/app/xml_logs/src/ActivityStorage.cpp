///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#include "ActivityStorage.hpp"
#include "EntryFormat.hpp"
#include "EntryStorage.hpp"
#include <eagine/overloaded.hpp>
#include <algorithm>

#include <QDebug>
//------------------------------------------------------------------------------
void ActivityStorage::beginStream(
  std::uintptr_t stream_id,
  const LogStreamInfo&) noexcept {
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
void ActivityStorage::addEntry(const LogEntryData& entry) noexcept {
    for(const auto& arg_info : entry.args) {
        std::visit<void>(
          eagine::overloaded(
            [&](const std::tuple<float, float, float>& mvm) {
                auto& activity = [&]() -> ActivityData& {
                    const auto entry_arg = std::get<0>(arg_info);
                    const auto pos = std::find_if(
                      _activities.begin(),
                      _activities.end(),
                      [&](const auto& existing) {
                          return (existing.stream_id == entry.stream_id) &&
                                 (existing.instance == entry.instance) &&
                                 (existing.source == entry.source) &&
                                 (existing.arg == entry_arg);
                      });
                    if(pos != _activities.end()) {
                        return *pos;
                    }
                    _activities.emplace_back();
                    auto& result = _activities.back();
                    result.stream_id = entry.stream_id;
                    result.instance = entry.instance;
                    result.source = entry.source;
                    result.arg = entry_arg;
                    return result;
                }();
                activity.message = EntryFormat().format(entry);
                activity.min = std::get<0>(mvm);
                activity.value = std::get<1>(mvm);
                activity.max = std::get<2>(mvm);
            },
            [](const auto&) {}),
          std::get<1>(std::get<1>(arg_info)));
    }
}
//------------------------------------------------------------------------------
