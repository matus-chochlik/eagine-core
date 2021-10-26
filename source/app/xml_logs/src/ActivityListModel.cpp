///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#include "ActivityListModel.hpp"
#include "Backend.hpp"
#include "EntryFormat.hpp"
#include "EntryLog.hpp"
#include "ProgressViewModel.hpp"
#include "Utility.hpp"
#include <eagine/extract.hpp>
#include <eagine/is_within_limits.hpp>
//------------------------------------------------------------------------------
ActivityListModel::ActivityListModel(ProgressViewModel& parent)
  : QAbstractItemModel{nullptr}
  , eagine::main_ctx_object{EAGINE_ID(ActListMdl), parent}
  , _parent{parent} {}
//------------------------------------------------------------------------------
auto ActivityListModel::backend() const noexcept -> Backend& {
    return _parent.backend();
}
//------------------------------------------------------------------------------
auto ActivityListModel::roleNames() const -> QHash<int, QByteArray> {
    QHash<int, QByteArray> result;
    result.insert(Qt::DisplayRole, "display");
    result.insert(activityMessage, "message");
    result.insert(activityFormat, "format");
    result.insert(activityStreamId, "streamId");
    result.insert(activityInstanceId, "instanceId");
    result.insert(activitySourceId, "sourceId");
    result.insert(activityTag, "tag");
    result.insert(activitySeverity, "severity");
    return result;
}
//------------------------------------------------------------------------------
auto ActivityListModel::columnCount(const QModelIndex&) const -> int {
    return 1;
}
//------------------------------------------------------------------------------
auto ActivityListModel::parent(const QModelIndex&) const -> QModelIndex {
    return {};
}
//------------------------------------------------------------------------------
auto ActivityListModel::index(int row, int column, const QModelIndex&) const
  -> QModelIndex {
    return QAbstractItemModel::createIndex(
      row, column, _parent.entryLog().getActivityData(row));
}
//------------------------------------------------------------------------------
auto ActivityListModel::rowCount(const QModelIndex& i) const -> int {
    return i.isValid() ? 0 : _parent.entryLog().getActivityCount();
}
//------------------------------------------------------------------------------
auto ActivityListModel::getActivityCount() const -> int {
    return _parent.entryLog().getActivityCount();
}
//------------------------------------------------------------------------------
auto ActivityListModel::getActivityMessage(const ActivityData&) const
  -> QString {
    return {}; // TODO
}
//------------------------------------------------------------------------------
auto ActivityListModel::getActivityFormat(const ActivityData&) const
  -> QString {
    return {}; // TODO
}
//------------------------------------------------------------------------------
auto ActivityListModel::getActivityStreamId(const ActivityData& entry) const
  -> qlonglong {
    return eagine::limit_cast<qlonglong>(entry.stream_id);
}
//------------------------------------------------------------------------------
auto ActivityListModel::getActivityInstanceId(const ActivityData& entry) const
  -> qlonglong {
    return eagine::limit_cast<qlonglong>(entry.instance);
}
//------------------------------------------------------------------------------
auto ActivityListModel::getActivitySourceId(const ActivityData& entry) const
  -> QString {
    return toQString(entry.source.name().view());
}
//------------------------------------------------------------------------------
auto ActivityListModel::getActivityTag(const ActivityData& entry) const
  -> QString {
    return toQString(entry.tag.name().view());
}
//------------------------------------------------------------------------------
auto ActivityListModel::getActivitySeverity(const ActivityData& entry) const
  -> QString {
    return toQString(eagine::enumerator_name(entry.severity));
}
//------------------------------------------------------------------------------
auto ActivityListModel::data(const QModelIndex& index, int role) const
  -> QVariant {
    if(auto optActivity{static_cast<ActivityData*>(index.internalPointer())}) {
        auto& activity{eagine::extract(optActivity)};
        switch(role) {
            case activityMessage:
                return {getActivityMessage(activity)};
            case activityFormat:
                return {getActivityFormat(activity)};
            case activityStreamId:
                return {getActivityStreamId(activity)};
            case activityInstanceId:
                return {getActivityInstanceId(activity)};
            case activitySourceId:
                return {getActivitySourceId(activity)};
            case activityTag:
                return {getActivityTag(activity)};
            case activitySeverity:
                return {getActivitySeverity(activity)};
            default:
                break;
        }
    }
    return {};
}
//------------------------------------------------------------------------------
