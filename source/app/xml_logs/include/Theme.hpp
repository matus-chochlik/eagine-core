///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#ifndef EAGINE_XML_LOGS_THEME
#define EAGINE_XML_LOGS_THEME

import eagine.core;
#include <QColor>
#include <QObject>

//------------------------------------------------------------------------------
class Theme
  : public QObject
  , public eagine::main_ctx_object {
    Q_OBJECT

    Q_PROPERTY(bool light READ getLight WRITE setLight NOTIFY lightChanged)
    Q_PROPERTY(int entrySourceWidth READ getEntrySourceWidth NOTIFY sizesChanged)
    Q_PROPERTY(
      int entrySeverityWidth READ getEntrySeverityWidth NOTIFY sizesChanged)
    Q_PROPERTY(
      int entryReltimeWidth READ getEntryReltimeWidth NOTIFY sizesChanged)
    Q_PROPERTY(
      int entryProgressWidth READ getEntryProgressWidth NOTIFY sizesChanged)
    Q_PROPERTY(
      int entryHeaderHeight READ getEntryHeaderHeight NOTIFY sizesChanged)

public:
    Theme(eagine::main_ctx_parent);

    void setLight(bool);
    auto getLight() const -> bool;
    auto getEntrySourceWidth() const -> int;
    auto getEntrySeverityWidth() const -> int;
    auto getEntryReltimeWidth() const -> int;
    auto getEntryProgressWidth() const -> int;
    auto getEntryHeaderHeight() const -> int;

    auto getSeverityColor(eagine::log_event_severity) -> QColor;

signals:
    void lightChanged();
    void sizesChanged();
public slots:
private:
    bool _light{false};
};
//------------------------------------------------------------------------------
#endif
