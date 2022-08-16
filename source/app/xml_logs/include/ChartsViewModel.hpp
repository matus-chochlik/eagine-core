///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#ifndef EAGINE_XML_LOG_CHARTS_VIEW_MODEL
#define EAGINE_XML_LOG_CHARTS_VIEW_MODEL

import eagine.core;
#include <QObject>

class EntryLog;
//------------------------------------------------------------------------------
class ChartsViewModel
  : public QObject
  , public eagine::main_ctx_object {
    Q_OBJECT

public:
    ChartsViewModel(EntryLog& parent);

signals:
public slots:

private:
    EntryLog& _parent;
};
//------------------------------------------------------------------------------
#endif
