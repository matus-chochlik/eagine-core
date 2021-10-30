///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#ifndef EAGINE_XML_LOG_PROGRESS_LIST_MODEL
#define EAGINE_XML_LOG_PROGRESS_LIST_MODEL

#include <eagine/main_ctx_object.hpp>
#include <QAbstractItemModel>

class Backend;
class ProgressViewModel;
struct ActivityData;
//------------------------------------------------------------------------------
class ActivityListModel
  : public QAbstractItemModel
  , public eagine::main_ctx_object {
    Q_OBJECT

    Q_PROPERTY(
      int activityCount READ getActivityCount NOTIFY activityCountChanged)
public:
    ActivityListModel(ProgressViewModel& parent);

    auto backend() const noexcept -> Backend&;

    auto roleNames() const -> QHash<int, QByteArray> final;
    auto index(int row, int column, const QModelIndex& parent = {}) const
      -> QModelIndex final;
    auto parent(const QModelIndex& child) const -> QModelIndex final;
    auto columnCount(const QModelIndex& parent) const -> int final;
    auto rowCount(const QModelIndex& parent) const -> int final;
    auto data(const QModelIndex& index, int role) const -> QVariant final;
    auto getActivityCount() const -> int;

    void handleActivitiesChanged();
signals:
    void activityCountChanged();
public slots:

private:
    enum ProgressAttributes : int {
        activityMessage = Qt::UserRole + 1,
        activityLogIdentity,
        activityStreamId,
        activityInstanceId,
        activitySourceId,
        activityArg,
        activityMin,
        activityMax,
        activityValue,
        activitySeverity,
        activitySeverityColor,
        activityRemainingTime
    };

    auto getActivityMessage(const ActivityData&) const -> QString;
    auto getActivityLogIdentity(const ActivityData&) const -> QString;
    auto getActivityStreamId(const ActivityData&) const -> qlonglong;
    auto getActivityInstanceId(const ActivityData&) const -> qlonglong;
    auto getActivitySourceId(const ActivityData&) const -> QString;
    auto getActivityArg(const ActivityData&) const -> QString;
    auto getActivityMin(const ActivityData&) const -> qreal;
    auto getActivityMax(const ActivityData&) const -> qreal;
    auto getActivityValue(const ActivityData&) const -> qreal;
    auto getActivitySeverity(const ActivityData&) const -> QString;
    auto getActivitySeverityColor(const ActivityData&) const -> QColor;
    auto getActivityRemainingTime(const ActivityData&) const -> QVariant;

    ProgressViewModel& _parent;
};
//------------------------------------------------------------------------------
#endif
