///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#ifndef EAGINE_XML_LOGS_THEME
#define EAGINE_XML_LOGS_THEME

#include <eagine/main_ctx_object.hpp>
#include <QObject>

//------------------------------------------------------------------------------
class Theme
  : public QObject
  , public eagine::main_ctx_object {
    Q_OBJECT

    Q_PROPERTY(bool light READ getLight WRITE setLight NOTIFY lightChanged)

public:
    Theme(eagine::main_ctx_parent);

    void setLight(bool);
    auto getLight() const -> bool;

signals:
    void lightChanged();
public slots:
private:
    bool _light{false};
};
//------------------------------------------------------------------------------
#endif
