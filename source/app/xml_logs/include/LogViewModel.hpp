///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#ifndef EAGINE_XML_LOG_LOG_VIEW_MODEL
#define EAGINE_XML_LOG_LOG_VIEW_MODEL

#include "EntryFilterModel.hpp"
#include <eagine/main_ctx_object.hpp>
#include <QObject>

class EntryLog;
//------------------------------------------------------------------------------
class LogViewModel
  : public QObject
  , public eagine::main_ctx_object {
    Q_OBJECT

public:
    LogViewModel(EntryLog& parent);

signals:
public slots:

private:
    EntryLog& _parent;
    EntryFilterModel _entryFilterModel;
};
//------------------------------------------------------------------------------
#endif
