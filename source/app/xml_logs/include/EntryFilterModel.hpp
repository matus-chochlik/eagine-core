///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#ifndef EAGINE_XML_LOG_ENTRY_FILTER_MODEL
#define EAGINE_XML_LOG_ENTRY_FILTER_MODEL

#include <eagine/main_ctx_object.hpp>
#include <QObject>

class LogViewModel;
//------------------------------------------------------------------------------
class EntryFilterModel
  : public QObject
  , public eagine::main_ctx_object {
    Q_OBJECT

public:
    EntryFilterModel(LogViewModel& parent);

signals:
public slots:

private:
    LogViewModel& _parent;
};
//------------------------------------------------------------------------------
#endif
