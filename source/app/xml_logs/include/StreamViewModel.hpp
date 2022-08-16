///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#ifndef EAGINE_XML_LOG_STREAM_VIEW_MODEL
#define EAGINE_XML_LOG_STREAM_VIEW_MODEL

import eagine.core;
#include "StreamListModel.hpp"
#include <QObject>

class EntryLog;
//------------------------------------------------------------------------------
class StreamViewModel
  : public QObject
  , public eagine::main_ctx_object {
    Q_OBJECT

    Q_PROPERTY(StreamListModel* streamList READ getStreamListModel CONSTANT)

public:
    StreamViewModel(EntryLog& parent);

    auto backend() const noexcept -> Backend&;
    auto entryLog() const noexcept -> EntryLog& {
        return _parent;
    }

    auto getStreamListModel() noexcept -> StreamListModel*;

signals:
public slots:
    void onStreamsAdded();

private:
    EntryLog& _parent;
    StreamListModel _streamListModel;
};
//------------------------------------------------------------------------------
#endif
