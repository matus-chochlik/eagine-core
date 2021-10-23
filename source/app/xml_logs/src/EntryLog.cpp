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
  , _entriesViewModel{*this}
  , _chartsViewModel{*this}
  , _progressViewModel{*this} {
    connect(
      this,
      &EntryLog::entriesAdded,
      &_entriesViewModel,
      &EntriesViewModel::onEntriesAdded);
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
void EntryLog::beginStream(std::uintptr_t streamId) {
    EAGINE_MAYBE_UNUSED(streamId);
}
//------------------------------------------------------------------------------
void EntryLog::endStream(std::uintptr_t streamId) {
    EAGINE_MAYBE_UNUSED(streamId);
}
//------------------------------------------------------------------------------
void EntryLog::addEntry(LogEntryData& entry) {
    EAGINE_ASSERT(_entries);
    _entries->addEntry(entry);
}
//------------------------------------------------------------------------------
void EntryLog::commitEntries() {
    const auto currEntryCount = _entries->entryCount();
    emit entriesAdded(_prevEntryCount, currEntryCount);
    _prevEntryCount = currEntryCount;
}
//------------------------------------------------------------------------------
auto EntryLog::getEntriesViewModel() noexcept -> EntriesViewModel* {
    return &_entriesViewModel;
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
auto EntryLog::getActivityCount() const noexcept -> int {
    return 0; // TODO
}
//------------------------------------------------------------------------------
auto EntryLog::getActivityData(int) noexcept -> ActivityData* {
    return nullptr; // TODO
}
//------------------------------------------------------------------------------
