///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#ifndef EAGINE_XML_LOG_ENTRY_FILTER_MODEL
#define EAGINE_XML_LOG_ENTRY_FILTER_MODEL

#if EAGINE_CORE_MODULE
import eagine.core;
#else
#include <eagine/main_ctx_object.hpp>
#endif
#include <QObject>

class EntryViewModel;
//------------------------------------------------------------------------------
class EntryFilterModel
  : public QObject
  , public eagine::main_ctx_object {
    Q_OBJECT

public:
    EntryFilterModel(EntryViewModel& parent);

signals:
public slots:

private:
    EntryViewModel& _parent;
};
//------------------------------------------------------------------------------
#endif
