///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#include "Connection.hpp"
#include "Backend.hpp"
#include <eagine/assert.hpp>
#include <eagine/from_string.hpp>
#include <eagine/reflect/enumerators.hpp>

//------------------------------------------------------------------------------
Connection::Connection(QTcpSocket& socket, Backend& parent)
  : QObject{nullptr}
  , eagine::main_ctx_object{"Connection", parent}
  , _backend{parent}
  , _socket{socket}
  , _streamId{reinterpret_cast<stream_id_t>(&_socket)} {

    connect(&_socket, &QTcpSocket::readyRead, this, &Connection::onReadyRead);
    connect(&_socket, &QTcpSocket::errorOccurred, this, &Connection::onError);

    _xmlReader.setDevice(&_socket);
}
//------------------------------------------------------------------------------
Connection::~Connection() noexcept {
    _socket.close();
}
//------------------------------------------------------------------------------
auto Connection::_toIdentifier(const QStringRef& s) noexcept
  -> eagine::identifier {
    return eagine::identifier{eagine::view(s.toUtf8())};
}
//------------------------------------------------------------------------------
auto Connection::_toFloat(const QStringRef& s, float f) noexcept -> float {
    return extract_or(eagine::from_string<float>(eagine::view(s.toUtf8())), f);
}
//------------------------------------------------------------------------------
auto Connection::_toInt(const QStringRef& s, int i) noexcept -> int {
    return extract_or(eagine::from_string<int>(eagine::view(s.toUtf8())), i);
}
//------------------------------------------------------------------------------
auto Connection::_toUnsigned64(const QStringRef& s, std::uint64_t f) noexcept
  -> std::uint64_t {
    return extract_or(
      eagine::from_string<std::uint64_t>(eagine::view(s.toUtf8())), f);
}
//------------------------------------------------------------------------------
auto Connection::_toSeverity(const QStringRef& s) noexcept
  -> eagine::log_event_severity {
    if(const auto severity{eagine::from_string<eagine::log_event_severity>(
         eagine::view(s.toUtf8()))}) {
        return extract(severity);
    }
    return eagine::log_event_severity::info;
}
//------------------------------------------------------------------------------
auto Connection::_cacheString(const QStringRef& s) noexcept
  -> eagine::string_view {
    const auto utf8{s.toUtf8()};
    return _backend.entryLog().cacheString(eagine::view(utf8));
}
//------------------------------------------------------------------------------
auto Connection::_isAtLogTag() const noexcept -> bool {
    return _xmlReader.name() == QLatin1String("log");
}
//------------------------------------------------------------------------------
auto Connection::_isAtMessageTag() const noexcept -> bool {
    return _xmlReader.name() == QLatin1String("m");
}
//------------------------------------------------------------------------------
auto Connection::_isAtFormatTag() const noexcept -> bool {
    return _xmlReader.name() == QLatin1String("f");
}
//------------------------------------------------------------------------------
auto Connection::_isAtArgumentTag() const noexcept -> bool {
    return _xmlReader.name() == QLatin1String("a");
}
//------------------------------------------------------------------------------
auto Connection::_handleBeginLog() noexcept -> bool {
    LogStreamInfo info{};
    QStringRef logIdentity = _xmlReader.attributes().value("identity");
    info.logIdentity = _cacheString(logIdentity);
    _backend.entryLog().beginStream(_streamId, info);
    return true;
}
//------------------------------------------------------------------------------
void Connection::_handleEndLog() noexcept {
    _backend.entryLog().endStream(_streamId);
}
//------------------------------------------------------------------------------
auto Connection::_handleBeginMessage() noexcept -> bool {
    // id
    _currentEntry.streamId = _streamId;
    _currentEntry.hasProgress = false;
    _currentMin = 0.F;
    _currentMax = 1.F;
    // tag
    QStringRef s = _xmlReader.attributes().value("tag");
    if(s.isEmpty()) {
        _currentEntry.tag = {};
    } else {
        _currentEntry.tag = _toIdentifier(s);
    }
    // source
    s = _xmlReader.attributes().value("src");
    if(s.isEmpty()) {
        _currentEntry.source = "Unknown";
    } else {
        _currentEntry.source = _toIdentifier(s);
    }
    // instance
    s = _xmlReader.attributes().value("iid");
    _currentEntry.instance = _toUnsigned64(s, 0);
    // severity
    s = _xmlReader.attributes().value("lvl");
    _currentEntry.severity = _toSeverity(s);
    // timestamp
    s = _xmlReader.attributes().value("ts");
    _currentEntry.reltimeSec = _toFloat(s, -1.F);

    return true;
}
//------------------------------------------------------------------------------
void Connection::_handleEndMessage() noexcept {
    _backend.entryLog().addEntry(_currentEntry);
}
//------------------------------------------------------------------------------
void Connection::_handleSpecialArgument() noexcept {
    if(_currentEntry.tag == "ProgArgs") {
        auto& info = _backend.entryLog().streamInfoRef(_streamId);
        if(_isArgName("arg")) {
            info.args.push_back(_cacheString(_xmlReader.text()));
        }
    } else if(_currentEntry.tag == "OSInfo") {
        auto& info = _backend.entryLog().streamInfoRef(_streamId);
        if(_isArgName("osName")) {
            info.osName = _cacheString(_xmlReader.text());
        } else if(_isArgName("osCodeName")) {
            info.osCodeName = _cacheString(_xmlReader.text());
        }
    } else if(_currentEntry.tag == "GitInfo") {
        auto& info = _backend.entryLog().streamInfoRef(_streamId);
        if(_isArgName("gitBranch")) {
            info.gitBranch = _cacheString(_xmlReader.text());
        } else if(_isArgName("gitHashId")) {
            info.gitHashId = _cacheString(_xmlReader.text());
        } else if(_isArgName("gitDate")) {
        } else if(_isArgName("gitDescrib")) {
            info.gitDescribe = _cacheString(_xmlReader.text());
        } else if(_isArgName("gitVersion")) {
            info.gitVersion = _cacheString(_xmlReader.text());
        }
    } else if(_currentEntry.tag == "Instance") {
        auto& info = _backend.entryLog().streamInfoRef(_streamId);
        if(_isArgName("instanceId")) {
            info.instanceId = _cacheString(_xmlReader.text());
        }
    } else if(_currentEntry.tag == "Compiler") {
        auto& info = _backend.entryLog().streamInfoRef(_streamId);
        if(_isArgName("archtcture")) {
            info.architecture = _cacheString(_xmlReader.text());
        } else if(_isArgName("complrName")) {
            info.compilerName = _cacheString(_xmlReader.text());
        } else if(_isArgName("complrMajr")) {
            info.compilerVersionMajor = _toInt(_xmlReader.text(), -1);
        } else if(_isArgName("complrMinr")) {
            info.compilerVersionMinor = _toInt(_xmlReader.text(), -1);
        } else if(_isArgName("complrPtch")) {
            info.compilerVersionPatch = _toInt(_xmlReader.text(), -1);
        }
    }
}
//------------------------------------------------------------------------------
auto Connection::_handleBeginArgument() noexcept -> bool {
    QStringRef s = _xmlReader.attributes().value("n");
    if(s.isEmpty()) {
        return false;
    }
    _currentArgName = _toIdentifier(s);
    // tag
    s = _xmlReader.attributes().value("t");
    if(!s.isEmpty()) {
        _currentArgTag = _toIdentifier(s);
    }
    // min
    s = _xmlReader.attributes().value("min");
    if(!s.isEmpty()) {
        _currentMin = _toFloat(s, 0.F);
    }
    // max
    s = _xmlReader.attributes().value("max");
    if(!s.isEmpty()) {
        _currentMax = _toFloat(s, 1.F);
    }
    return true;
}
//------------------------------------------------------------------------------
void Connection::_handleStartElement() noexcept {
    if(_isAtArgumentTag()) {
        _readingArgument = _handleBeginArgument();
    } else if(_isAtMessageTag()) {
        _readingMessage = _handleBeginMessage();
    } else if(_isAtFormatTag()) {
        _readingFormat = true;
    } else if(_isAtLogTag()) {
        _readingLog = _handleBeginLog();
    }
}
//------------------------------------------------------------------------------
void Connection::_handleEndElement() noexcept {
    if(_isAtArgumentTag()) {
        if(_readingArgument) {
            _readingArgument = false;
        }
    } else if(_isAtMessageTag()) {
        if(_readingMessage) {
            _readingMessage = false;
            _handleEndMessage();
        }
    } else if(_isAtFormatTag()) {
        _readingFormat = false;
    } else if(_isAtLogTag()) {
        _readingLog = false;
        _handleEndLog();
    }
}
//------------------------------------------------------------------------------
void Connection::_handleFormatText() noexcept {
    _currentEntry.format = _cacheString(_xmlReader.text());
}
//------------------------------------------------------------------------------
auto Connection::_isArgName(eagine::identifier name) const noexcept -> bool {
    return _currentArgName == name;
}
//------------------------------------------------------------------------------
auto Connection::_isArgTag(eagine::identifier tag) const noexcept -> bool {
    return _currentArgTag == tag;
}
//------------------------------------------------------------------------------
auto Connection::_isProgressArg() const noexcept -> bool {
    return _isArgTag("Progress");
}
//------------------------------------------------------------------------------
auto Connection::_isBoolArg() const noexcept -> bool {
    return _isArgTag("bool");
}
//------------------------------------------------------------------------------
auto Connection::_isIntegerArg() const noexcept -> bool {
    return _isArgTag("int64") || _isArgTag("int32") || _isArgTag("int16");
}
//------------------------------------------------------------------------------
auto Connection::_isUnsignedArg() const noexcept -> bool {
    return _isArgTag("uint64") || _isArgTag("uint32") || _isArgTag("uint16");
}
//------------------------------------------------------------------------------
auto Connection::_isFloatArg() const noexcept -> bool {
    return _isArgTag("real");
}
//------------------------------------------------------------------------------
auto Connection::_isDurationArg() const noexcept -> bool {
    return _isArgTag("duration");
}
//------------------------------------------------------------------------------
void Connection::_handleArgumentValue() noexcept {
    const auto s = _xmlReader.text();
    if(_isProgressArg()) {
        _currentEntry.hasProgress |= true;
        if(const auto value{
             eagine::from_string<std::uintmax_t>(eagine::view(s.toUtf8()))}) {
            _currentEntry.args[_currentArgName] = {
              _currentArgTag,
              std::tuple<float, float, float>{
                _currentMin, extract(value), _currentMax}};
        }
    } else if(_isBoolArg()) {
        _currentEntry.args[_currentArgName] = {
          _currentArgTag, s == QLatin1String("true")};
    } else if(_isUnsignedArg()) {
        if(const auto value{
             eagine::from_string<std::uintmax_t>(eagine::view(s.toUtf8()))}) {
            _currentEntry.args[_currentArgName] = {
              _currentArgTag, extract(value)};
        }
    } else if(_isIntegerArg()) {
        if(const auto value{
             eagine::from_string<std::intmax_t>(eagine::view(s.toUtf8()))}) {
            _currentEntry.args[_currentArgName] = {
              _currentArgTag, extract(value)};
        }
    } else if(_isFloatArg()) {
        if(const auto value{
             eagine::from_string<float>(eagine::view(s.toUtf8()))}) {
            _currentEntry.args[_currentArgName] = {
              _currentArgTag, extract(value)};
        }
    } else if(_isDurationArg()) {
        if(const auto value{
             eagine::from_string<float>(eagine::view(s.toUtf8()))}) {
            _currentEntry.args[_currentArgName] = {
              _currentArgTag, std::chrono::duration<float>{extract(value)}};
        }
    } else {
        _currentEntry.args[_currentArgName] = {_currentArgTag, _cacheString(s)};
    }
    _handleSpecialArgument();
}
//------------------------------------------------------------------------------
void Connection::_handleElementText() noexcept {
    if(_readingMessage) {
        if(_readingFormat) {
            _handleFormatText();
        } else if(_readingArgument) {
            _handleArgumentValue();
        }
    }
}
//------------------------------------------------------------------------------
void Connection::onReadyRead() {
    bool keepReading = _readingLog;
    do {
        switch(_xmlReader.readNext()) {
            case QXmlStreamReader::StartElement: {
                _handleStartElement();
                break;
            }
            case QXmlStreamReader::EndElement: {
                _handleEndElement();
                break;
            }
            case QXmlStreamReader::Characters: {
                _handleElementText();
                break;
            }
            case QXmlStreamReader::StartDocument: {
                keepReading = true;
                break;
            }
            case QXmlStreamReader::NoToken:
            case QXmlStreamReader::Invalid:
            case QXmlStreamReader::EndDocument: {
                keepReading = false;
                break;
            }
            default: {
                break;
            }
        }
    } while(keepReading);
    _backend.entryLog().commitEntries();
}
//------------------------------------------------------------------------------
void Connection::onError(QAbstractSocket::SocketError) {
    _self.reset();
}
//------------------------------------------------------------------------------
