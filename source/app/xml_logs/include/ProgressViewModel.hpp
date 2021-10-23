///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#ifndef EAGINE_XML_LOG_PROGRESS_VIEW_MODEL
#define EAGINE_XML_LOG_PROGRESS_VIEW_MODEL

#include "ActivityListModel.hpp"
#include <eagine/main_ctx_object.hpp>
#include <QObject>

class EntryLog;
//------------------------------------------------------------------------------
class ProgressViewModel
  : public QObject
  , public eagine::main_ctx_object {
    Q_OBJECT

    Q_PROPERTY(
      ActivityListModel* activityList READ getActivityListModel CONSTANT)

public:
    ProgressViewModel(EntryLog& parent);

    auto backend() const noexcept -> Backend&;
    auto entryLog() const noexcept -> EntryLog& {
        return _parent;
    }

    auto getActivityListModel() noexcept -> ActivityListModel*;

signals:
public slots:

private:
    EntryLog& _parent;
    ActivityListModel _activityListModel;
};
//------------------------------------------------------------------------------
#endif
