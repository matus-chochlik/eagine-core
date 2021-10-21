///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#ifndef EAGINE_XML_LOGS_CONNECTION
#define EAGINE_XML_LOGS_CONNECTION

#include "EntryStorage.hpp"
#include <eagine/main_ctx_object.hpp>
#include <QObject>
#include <QTcpSocket>
#include <QXmlStreamReader>
#include <memory>

//------------------------------------------------------------------------------
class Backend;
class Connection
  : public QObject
  , public eagine::main_ctx_object {
    Q_OBJECT

public:
    Connection(QTcpSocket&, Backend&);
    ~Connection() noexcept;

    void adoptSelf(std::shared_ptr<Connection> self) {
        _self = std::move(self);
    }
signals:
public slots:
    void onReadyRead();
    void onError(QAbstractSocket::SocketError);

private:
    auto _toIdentifier(const QStringRef&) -> eagine::identifier;
    auto _cacheString(const QStringRef&) -> eagine::string_view;

    Backend& _backend;
    std::shared_ptr<Connection> _self;
    QTcpSocket& _socket;
    QXmlStreamReader _xmlReader;
    const std::uintptr_t _streamId;
    LogEntryData _currentEntry;
    bool _readingLog{false};
    bool _readingMessage{false};
    bool _readingFormat{false};
    bool _readingAttribute{false};
};
//------------------------------------------------------------------------------
#endif
