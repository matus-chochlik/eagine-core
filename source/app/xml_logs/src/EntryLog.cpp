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
  , _logViewModel{*this}
  , _chartsViewModel{*this}
  , _progressViewModel{*this} {}
//------------------------------------------------------------------------------
void EntryLog::assignStorage(std::shared_ptr<LogEntryStorage> entries) {
    _entries = std::move(entries);
}
//------------------------------------------------------------------------------
auto EntryLog::entries() -> LogEntryStorage& {
    EAGINE_ASSERT(_entries);
    return *_entries;
}
//------------------------------------------------------------------------------
