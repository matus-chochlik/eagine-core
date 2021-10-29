///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#include "EntryLog.hpp"
#include "Backend.hpp"
//------------------------------------------------------------------------------
EntryLog::EntryLog(Backend& backend)
  : QObject{nullptr}
  , eagine::main_ctx_object{EAGINE_ID(EntryLog), backend}
  , _backend{backend}
  , _entryViewModel{*this}
  , _chartsViewModel{*this}
  , _progressViewModel{*this} {
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
    EAGINE_ASSERT(_entries);
    emit entriesAdded(0, _entries->entryCount());
    _prevEntryCount = _entries->entryCount();
}
//------------------------------------------------------------------------------
void EntryLog::assignStorage(std::shared_ptr<ActivityStorage> activities) {
    _activities = std::move(activities);
    EAGINE_ASSERT(_activities);
}
//------------------------------------------------------------------------------
auto EntryLog::cacheString(eagine::string_view s) -> eagine::string_view {
    EAGINE_ASSERT(_entries);
    return _entries->cacheString(s);
}
//------------------------------------------------------------------------------
void EntryLog::beginStream(std::uintptr_t streamId, const LogStreamInfo& info) {
    EAGINE_ASSERT(_entries);
    _entries->beginStream(streamId, info);
    EAGINE_ASSERT(_activities);
    _activities->beginStream(streamId, info);
}
//------------------------------------------------------------------------------
void EntryLog::endStream(std::uintptr_t streamId) {
    EAGINE_ASSERT(_activities);
    _activities->endStream(streamId);
    EAGINE_ASSERT(_entries);
    _entries->endStream(streamId);
    commitEntries();
}
//------------------------------------------------------------------------------
void EntryLog::addEntry(LogEntryData& entry) {
    if(entry.has_progress) {
        EAGINE_ASSERT(_activities);
        _activities->addEntry(entry);
    }
    EAGINE_ASSERT(_entries);
    _entries->addEntry(std::move(entry));
}
//------------------------------------------------------------------------------
void EntryLog::commitEntries() {
    const auto currEntryCount = _entries->entryCount();
    emit entriesAdded(_prevEntryCount, currEntryCount);
    _prevEntryCount = currEntryCount;
}
//------------------------------------------------------------------------------
auto EntryLog::getEntryViewModel() noexcept -> EntryViewModel* {
    return &_entryViewModel;
}
//------------------------------------------------------------------------------
auto EntryLog::getChartsViewModel() noexcept -> ChartsViewModel* {
    return &_chartsViewModel;
}
//------------------------------------------------------------------------------
auto EntryLog::getProgressViewModel() noexcept -> ProgressViewModel* {
    return &_progressViewModel;
}
//------------------------------------------------------------------------------
auto EntryLog::getEntryCount() const noexcept -> int {
    return _prevEntryCount;
}
//------------------------------------------------------------------------------
auto EntryLog::getEntryData(int index) noexcept -> LogEntryData* {
    EAGINE_ASSERT(_entries);
    return _entries->getEntry(index);
}
//------------------------------------------------------------------------------
auto EntryLog::getEntryConnectors(const LogEntryData& entry) noexcept
  -> LogEntryConnectors {
    EAGINE_ASSERT(_entries);
    return _entries->getEntryConnectors(entry);
}
//------------------------------------------------------------------------------
auto EntryLog::getActivityCount() const noexcept -> int {
    EAGINE_ASSERT(_activities);
    return _activities->activityCount();
}
//------------------------------------------------------------------------------
auto EntryLog::getActivityData(int index) noexcept -> ActivityData* {
    EAGINE_ASSERT(_activities);
    return _activities->getActivity(index);
}
//------------------------------------------------------------------------------
auto EntryLog::getStreamInfo(const LogEntryData& entry) noexcept
  -> const LogStreamInfo& {
    EAGINE_ASSERT(_entries);
    return _entries->getStreamInfo(entry);
}
//------------------------------------------------------------------------------
