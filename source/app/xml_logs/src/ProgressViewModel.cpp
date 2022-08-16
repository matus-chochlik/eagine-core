///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#include "ProgressViewModel.hpp"
#include "EntryLog.hpp"
//------------------------------------------------------------------------------
ProgressViewModel::ProgressViewModel(EntryLog& parent)
  : QObject{nullptr}
  , eagine::main_ctx_object{"ProgressVM", parent}
  , _parent{parent}
  , _activityListModel{*this} {}
//------------------------------------------------------------------------------
auto ProgressViewModel::backend() const noexcept -> Backend& {
    return entryLog().backend();
}
//------------------------------------------------------------------------------
auto ProgressViewModel::getActivityListModel() noexcept -> ActivityListModel* {
    return &_activityListModel;
}
//------------------------------------------------------------------------------
void ProgressViewModel::onEntriesAdded(int, int) {
    _activityListModel.handleActivitiesChanged();
}
//------------------------------------------------------------------------------
