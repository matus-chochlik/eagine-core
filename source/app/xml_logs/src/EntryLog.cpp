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
  , _progressViewModel{*this} {}
//------------------------------------------------------------------------------
void EntryLog::assignStorage(std::shared_ptr<LogEntryStorage> entries) {
    _entries = std::move(entries);
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
    EAGINE_ASSERT(_entries);
    return _entries->entryCount();
}
//------------------------------------------------------------------------------
auto EntryLog::getEntryData(int index) noexcept -> LogEntryData* {
    EAGINE_ASSERT(_entries);
    return _entries->getEntry(index);
}
//------------------------------------------------------------------------------
