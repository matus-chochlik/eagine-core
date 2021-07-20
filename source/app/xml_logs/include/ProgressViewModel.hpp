///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#ifndef EAGINE_XML_LOG_PROGRESS_VIEW_MODEL
#define EAGINE_XML_LOG_PROGRESS_VIEW_MODEL

#include <eagine/main_ctx_object.hpp>
#include <QObject>

class EntryLog;
//------------------------------------------------------------------------------
class ProgressViewModel
  : public QObject
  , public eagine::main_ctx_object {
    Q_OBJECT

public:
    ProgressViewModel(EntryLog& parent);

signals:
public slots:

private:
    EntryLog& _parent;
};
//------------------------------------------------------------------------------
#endif
