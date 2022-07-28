///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#ifndef EAGINE_XML_LOGS_CONNECTION
#define EAGINE_XML_LOGS_CONNECTION

#if EAGINE_CORE_MODULE
import eagine.core;
import <memory>;
#else
#include <eagine/main_ctx_object.hpp>
#include <memory>
#endif
#include "EntryStorage.hpp"
#include <QObject>
#include <QTcpSocket>
#include <QXmlStreamReader>

//------------------------------------------------------------------------------
class Backend;
class Connection final
  : public QObject
  , public eagine::main_ctx_object {
    Q_OBJECT

public:
    Connection(QTcpSocket&, Backend&);
    ~Connection() noexcept final;

    void adoptSelf(std::shared_ptr<Connection> self) {
        _self = std::move(self);
    }
signals:
public slots:
    void onReadyRead();
    void onError(QAbstractSocket::SocketError);

private:
    auto _toIdentifier(const QStringRef&) noexcept -> eagine::identifier;
    auto _toFloat(const QStringRef&, float) noexcept -> float;
    auto _toInt(const QStringRef&, int) noexcept -> int;
    auto _toUnsigned64(const QStringRef&, std::uint64_t) noexcept
      -> std::uint64_t;
    auto _toSeverity(const QStringRef&) noexcept -> eagine::log_event_severity;
    auto _cacheString(const QStringRef&) noexcept -> eagine::string_view;

    auto _isAtLogTag() const noexcept -> bool;
    auto _isAtMessageTag() const noexcept -> bool;
    auto _isAtFormatTag() const noexcept -> bool;
    auto _isAtArgumentTag() const noexcept -> bool;

    auto _isArgName(eagine::identifier) const noexcept -> bool;
    auto _isArgTag(eagine::identifier) const noexcept -> bool;
    auto _isProgressArg() const noexcept -> bool;
    auto _isBoolArg() const noexcept -> bool;
    auto _isIntegerArg() const noexcept -> bool;
    auto _isUnsignedArg() const noexcept -> bool;
    auto _isFloatArg() const noexcept -> bool;
    auto _isDurationArg() const noexcept -> bool;

    auto _handleBeginLog() noexcept -> bool;
    void _handleEndLog() noexcept;
    auto _handleBeginMessage() noexcept -> bool;
    void _handleEndMessage() noexcept;
    void _handleSpecialArgument() noexcept;
    auto _handleBeginArgument() noexcept -> bool;
    void _handleStartElement() noexcept;
    void _handleEndElement() noexcept;
    void _handleFormatText() noexcept;
    void _handleArgumentValue() noexcept;
    void _handleElementText() noexcept;

    Backend& _backend;
    std::shared_ptr<Connection> _self;
    QTcpSocket& _socket;
    QXmlStreamReader _xmlReader;
    const stream_id_t _streamId;
    LogEntryData _currentEntry;
    eagine::identifier _currentArgName;
    eagine::identifier _currentArgTag;
    float _currentMin{0.F};
    float _currentMax{1.F};
    bool _readingLog{false};
    bool _readingMessage{false};
    bool _readingFormat{false};
    bool _readingArgument{false};
};
//------------------------------------------------------------------------------
#endif
