///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#include "EntriesViewModel.hpp"
#include "EntryLog.hpp"
//------------------------------------------------------------------------------
EntriesViewModel::EntriesViewModel(EntryLog& parent)
  : QObject{nullptr}
  , eagine::main_ctx_object{EAGINE_ID(LogVM), parent}
  , _parent{parent}
  , _entryFilterModel{*this}
  , _entryListModel{*this} {}
//------------------------------------------------------------------------------
auto EntriesViewModel::getEntryFilterModel() noexcept -> EntryFilterModel* {
    return &_entryFilterModel;
}
//------------------------------------------------------------------------------
auto EntriesViewModel::getEntryListModel() noexcept -> EntryListModel* {
    return &_entryListModel;
}
//------------------------------------------------------------------------------
