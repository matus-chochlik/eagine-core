///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

import eagine.core;
import <algorithm>;
import <numeric>;
#include "ActivityStorage.hpp"
#include "EntryFormat.hpp"
#include "EntryStorage.hpp"

//------------------------------------------------------------------------------
auto ActivityData::init(
  const LogEntryData& entry,
  const eagine::identifier entryArg) noexcept -> ActivityData& {
    streamId = entry.streamId;
    instance = entry.instance;
    source = entry.source;
    arg = entryArg;
    severity = entry.severity;
    startTime = std::chrono::steady_clock::now();
    updateTime = startTime;
    remainingUpdatePos = 0U;
    return *this;
}
//------------------------------------------------------------------------------
auto ActivityData::update(
  const LogEntryData& entry,
  const std::tuple<float, float, float>& mvm) noexcept -> ActivityData& {
    message = EntryFormat().format(entry);
    if(value > std::get<1>(mvm)) {
        startTime = std::chrono::steady_clock::now();
        updateTime = startTime;
        severity = entry.severity;
        remainingUpdatePos = 0U;
    } else {
        updateTime = std::chrono::steady_clock::now();
    }
    min = std::get<0>(mvm);
    value = std::get<1>(mvm);
    max = std::get<2>(mvm);
    if((max > min) && (value > min) && timeSinceStart() > std::chrono::seconds(5)) {
        remainingTimes[remainingUpdatePos++ % remainingTimes.size()] =
          timeSinceStart().count() * (todoRatio() / doneRatio());
    }

    return *this;
}
//------------------------------------------------------------------------------
auto ActivityData::doneRatio() const noexcept -> float {
    return (value - min) / (max - min);
}
//------------------------------------------------------------------------------
auto ActivityData::todoRatio() const noexcept -> float {
    return 1.F - doneRatio();
}
//------------------------------------------------------------------------------
auto ActivityData::timeSinceStart() const noexcept
  -> std::chrono::duration<float> {
    return std::chrono::steady_clock::now() - startTime;
}
//------------------------------------------------------------------------------
auto ActivityData::timeSinceUpdate() const noexcept
  -> std::chrono::duration<float> {
    return std::chrono::steady_clock::now() - updateTime;
}
//------------------------------------------------------------------------------
auto ActivityData::hasTimeEstimation() const noexcept -> bool {
    return remainingUpdatePos > remainingTimes.size() / 2U;
}
//------------------------------------------------------------------------------
auto ActivityData::estimatedRemainingTime() const noexcept
  -> std::chrono::duration<float> {
    if(hasTimeEstimation()) {
        const auto count = std::min(remainingTimes.size(), remainingUpdatePos);
        return std::chrono::duration<float>{
          std::accumulate(
            remainingTimes.begin(), remainingTimes.begin() + count, 0.F) /
          float(count)};
    }
    return {};
}
//------------------------------------------------------------------------------
auto ActivityData::isDone() const noexcept -> bool {
    return (todoRatio() < 0.001F);
}
//------------------------------------------------------------------------------
// storage
//------------------------------------------------------------------------------
void ActivityStorage::beginStream(
  [[maybe_unused]] stream_id_t streamId,
  const LogStreamInfo&) noexcept {}
//------------------------------------------------------------------------------
void ActivityStorage::endStream(stream_id_t streamId) noexcept {
    std::erase_if(_activities, [streamId](const auto& activity) -> bool {
        return activity.streamId == streamId;
    });
}
//------------------------------------------------------------------------------
auto ActivityStorage::_getEntryActivity(
  const LogEntryData& entry,
  const eagine::identifier entryArg) noexcept -> ActivityData& {
    const auto pos = std::find_if(
      _activities.begin(), _activities.end(), [&](const auto& existing) {
          return (existing.streamId == entry.streamId) &&
                 (existing.instance == entry.instance) &&
                 (existing.source == entry.source) &&
                 (existing.arg == entryArg);
      });
    if(pos != _activities.end()) {
        return *pos;
    }
    _activities.emplace_back();
    return _activities.back().init(entry, entryArg);
}
//------------------------------------------------------------------------------
void ActivityStorage::addEntry(const LogEntryData& entry) noexcept {
    for(const auto& arg_info : entry.args) {
        std::visit<void>(
          eagine::overloaded(
            [&](const std::tuple<float, float, float>& mvm) {
                _getEntryActivity(entry, std::get<0>(arg_info))
                  .update(entry, mvm);
            },
            [](const auto&) {}),
          std::get<1>(std::get<1>(arg_info)));
    }
}
//------------------------------------------------------------------------------
auto ActivityStorage::cleanupDone() noexcept -> bool {
    return std::erase_if(_activities, [](const auto& activity) -> bool {
               return activity.isDone();
           }) > 0U;
}
//------------------------------------------------------------------------------
