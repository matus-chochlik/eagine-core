///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#include "EntryLog.hpp"
#include "Backend.hpp"
#include <cassert>
//------------------------------------------------------------------------------
EntryLog::EntryLog(Backend& backend)
  : QObject{nullptr}
  , eagine::main_ctx_object{"EntryLog", backend}
  , _backend{backend}
  , _entryViewModel{*this}
  , _progressViewModel{*this}
  , _chartsViewModel{*this}
  , _streamViewModel{*this} {
    connect(
      this,
      &EntryLog::streamsAdded,
      &_streamViewModel,
      &StreamViewModel::onStreamsAdded);
    connect(
      this,
      &EntryLog::entriesAdded,
      &_entryViewModel,
      &EntryViewModel::onEntriesAdded);
    connect(
      this,
      &EntryLog::entriesAdded,
      &_progressViewModel,
      &ProgressViewModel::onEntriesAdded);
}
//------------------------------------------------------------------------------
void EntryLog::assignStorage(std::shared_ptr<LogEntryStorage> entries) {
    _entries = std::move(entries);
    assert(_entries);
    emit streamsAdded();
    emit entriesAdded(0, _entries->entryCount());
    _prevEntryCount = _entries->entryCount();
}
//------------------------------------------------------------------------------
void EntryLog::assignStorage(std::shared_ptr<ActivityStorage> activities) {
    _activities = std::move(activities);
    assert(_activities);
}
//------------------------------------------------------------------------------
auto EntryLog::cacheString(eagine::string_view s) -> eagine::string_view {
    assert(_entries);
    return _entries->cacheString(s);
}
//------------------------------------------------------------------------------
void EntryLog::beginStream(stream_id_t streamId, const LogStreamInfo& info) {
    assert(_entries);
    _entries->beginStream(streamId, info);
    assert(_activities);
    _activities->beginStream(streamId, info);
}
//------------------------------------------------------------------------------
void EntryLog::endStream(stream_id_t streamId) {
    assert(_activities);
    _activities->endStream(streamId);
    assert(_entries);
    _entries->endStream(streamId);
    commitEntries();
}
//------------------------------------------------------------------------------
void EntryLog::addEntry(LogEntryData& entry) {
    if(entry.hasProgress) {
        assert(_activities);
        _activities->addEntry(entry);
    }
    assert(_entries);
    _entries->addEntry(std::move(entry));
}
//------------------------------------------------------------------------------
void EntryLog::commitEntries() {
    emit streamsAdded();
    const auto currEntryCount = _entries->entryCount();
    emit entriesAdded(_prevEntryCount, currEntryCount);
    _prevEntryCount = currEntryCount;
}
//------------------------------------------------------------------------------
auto EntryLog::getEntryViewModel() noexcept -> EntryViewModel* {
    return &_entryViewModel;
}
//------------------------------------------------------------------------------
auto EntryLog::getProgressViewModel() noexcept -> ProgressViewModel* {
    return &_progressViewModel;
}
//------------------------------------------------------------------------------
auto EntryLog::getChartsViewModel() noexcept -> ChartsViewModel* {
    return &_chartsViewModel;
}
//------------------------------------------------------------------------------
auto EntryLog::getStreamViewModel() noexcept -> StreamViewModel* {
    return &_streamViewModel;
}
//------------------------------------------------------------------------------
auto EntryLog::getStreamCount() const noexcept -> int {
    assert(_entries);
    return _entries->streamCount();
}
//------------------------------------------------------------------------------
auto EntryLog::getStreamInfo(int index) noexcept -> LogStreamInfo* {
    assert(_entries);
    return _entries->getStreamInfo(index);
}
//------------------------------------------------------------------------------
auto EntryLog::streamInfoRef(const stream_id_t streamId) noexcept
  -> LogStreamInfo& {
    assert(_entries);
    return _entries->streamInfoRef(streamId);
}
//------------------------------------------------------------------------------
auto EntryLog::getEntryCount() const noexcept -> int {
    return _prevEntryCount;
}
//------------------------------------------------------------------------------
auto EntryLog::getEntryData(int index) noexcept -> LogEntryData* {
    assert(_entries);
    return _entries->getEntry(index);
}
//------------------------------------------------------------------------------
auto EntryLog::getEntryConnectors(const LogEntryData& entry) noexcept
  -> LogEntryConnectors {
    assert(_entries);
    return _entries->getEntryConnectors(entry);
}
//------------------------------------------------------------------------------
auto EntryLog::getActivityCount() const noexcept -> int {
    assert(_activities);
    return _activities->activityCount();
}
//------------------------------------------------------------------------------
auto EntryLog::getActivityData(int index) noexcept -> ActivityData* {
    assert(_activities);
    return _activities->getActivity(index);
}
//------------------------------------------------------------------------------
auto EntryLog::cleanupDoneActivities() noexcept -> bool {
    assert(_activities);
    return _activities->cleanupDone();
}
//------------------------------------------------------------------------------
