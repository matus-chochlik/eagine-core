///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#include "EntryViewModel.hpp"
#include "EntryLog.hpp"
//------------------------------------------------------------------------------
EntryViewModel::EntryViewModel(EntryLog& parent)
  : QObject{nullptr}
  , eagine::main_ctx_object{"LogVM", parent}
  , _parent{parent}
  , _entryFilterModel{*this}
  , _entryListModel{*this} {}
//------------------------------------------------------------------------------
auto EntryViewModel::backend() const noexcept -> Backend& {
    return entryLog().backend();
}
//------------------------------------------------------------------------------
auto EntryViewModel::getEntryFilterModel() noexcept -> EntryFilterModel* {
    return &_entryFilterModel;
}
//------------------------------------------------------------------------------
auto EntryViewModel::getEntryListModel() noexcept -> EntryListModel* {
    return &_entryListModel;
}
//------------------------------------------------------------------------------
void EntryViewModel::onEntriesAdded(int previous, int current) {
    _entryListModel.handleEntriesAdded(previous, current);
}
//------------------------------------------------------------------------------
