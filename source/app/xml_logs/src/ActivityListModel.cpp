///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#include "ActivityListModel.hpp"
#include "Backend.hpp"
#include "EntryLog.hpp"
#include "ProgressViewModel.hpp"
#include "Utility.hpp"
#include <eagine/extract.hpp>
#include <eagine/is_within_limits.hpp>
//------------------------------------------------------------------------------
ActivityListModel::ActivityListModel(ProgressViewModel& parent)
  : QAbstractItemModel{nullptr}
  , eagine::main_ctx_object{EAGINE_ID(ActListMdl), parent}
  , _parent{parent}
  , _timerId{startTimer(2000)} {}
//------------------------------------------------------------------------------
ActivityListModel::~ActivityListModel() noexcept {
    killTimer(_timerId);
}
//------------------------------------------------------------------------------
auto ActivityListModel::backend() const noexcept -> Backend& {
    return _parent.backend();
}
//------------------------------------------------------------------------------
auto ActivityListModel::roleNames() const -> QHash<int, QByteArray> {
    QHash<int, QByteArray> result;
    result.insert(Qt::DisplayRole, "display");
    result.insert(activityMessage, "message");
    result.insert(activityLogIdentity, "logIdentity");
    result.insert(activityStreamId, "streamId");
    result.insert(activityInstanceId, "instanceId");
    result.insert(activitySourceId, "sourceId");
    result.insert(activityArg, "arg");
    result.insert(activityMin, "progressMin");
    result.insert(activityMax, "progressMax");
    result.insert(activityValue, "progressValue");
    result.insert(activitySeverity, "severity");
    result.insert(activitySeverityColor, "severityColor");
    result.insert(activityElapsedTime, "elapsedTime");
    result.insert(activityRemainingTime, "remainingTime");
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
void ActivityListModel::timerEvent(QTimerEvent*) {
    if(_parent.entryLog().cleanupDoneActivities()) {
        emit modelReset({});
        emit activityCountChanged();
    }
}
//------------------------------------------------------------------------------
auto ActivityListModel::getActivityMessage(const ActivityData& entry) const
  -> QString {
    return toQString(entry.message);
}
//------------------------------------------------------------------------------
auto ActivityListModel::getActivityLogIdentity(const ActivityData& entry) const
  -> QString {
    return toQString(
      _parent.entryLog().getStreamInfo(entry.stream_id).log_identity);
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
auto ActivityListModel::getActivityArg(const ActivityData& entry) const
  -> QString {
    return toQString(entry.arg.name().view());
}
//------------------------------------------------------------------------------
auto ActivityListModel::getActivityMin(const ActivityData& entry) const
  -> qreal {
    return entry.min;
}
//------------------------------------------------------------------------------
auto ActivityListModel::getActivityMax(const ActivityData& entry) const
  -> qreal {
    return entry.max;
}
//------------------------------------------------------------------------------
auto ActivityListModel::getActivityValue(const ActivityData& entry) const
  -> qreal {
    return entry.value;
}
//------------------------------------------------------------------------------
auto ActivityListModel::getActivitySeverity(const ActivityData& entry) const
  -> QString {
    return toQString(eagine::enumerator_name(entry.severity));
}
//------------------------------------------------------------------------------
auto ActivityListModel::getActivitySeverityColor(const ActivityData& entry) const
  -> QColor {
    return backend().theme().getSeverityColor(entry.severity);
}
//------------------------------------------------------------------------------
auto ActivityListModel::getActivityElapsedTime(const ActivityData& entry) const
  -> QVariant {
    return {entry.timeSinceStart().count()};
}
//------------------------------------------------------------------------------
auto ActivityListModel::getActivityRemainingTime(const ActivityData& entry) const
  -> QVariant {
    if(entry.hasTimeEstimation()) {
        return {entry.estimatedRemainingTime().count()};
    }
    return {};
}
//------------------------------------------------------------------------------
auto ActivityListModel::data(const QModelIndex& index, int role) const
  -> QVariant {
    if(auto optActivity{static_cast<ActivityData*>(index.internalPointer())}) {
        auto& activity{eagine::extract(optActivity)};
        switch(role) {
            case activityMessage:
                return {getActivityMessage(activity)};
            case activityLogIdentity:
                return {getActivityLogIdentity(activity)};
            case activityStreamId:
                return {getActivityStreamId(activity)};
            case activityInstanceId:
                return {getActivityInstanceId(activity)};
            case activitySourceId:
                return {getActivitySourceId(activity)};
            case activityArg:
                return {getActivityArg(activity)};
            case activityMin:
                return {getActivityMin(activity)};
            case activityMax:
                return {getActivityMax(activity)};
            case activityValue:
                return {getActivityValue(activity)};
            case activitySeverity:
                return {getActivitySeverity(activity)};
            case activitySeverityColor:
                return {getActivitySeverityColor(activity)};
            case activityElapsedTime:
                return {getActivityElapsedTime(activity)};
            case activityRemainingTime:
                return {getActivityRemainingTime(activity)};
            default:
                break;
        }
    }
    return {};
}
//------------------------------------------------------------------------------
void ActivityListModel::handleActivitiesChanged() {
    emit modelReset({});
    emit activityCountChanged();
}
//------------------------------------------------------------------------------

