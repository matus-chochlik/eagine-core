///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#ifndef EAGINE_XML_LOGS_BACKEND
#define EAGINE_XML_LOGS_BACKEND

#include "Theme.hpp"
#include <eagine/main_ctx_object.hpp>
#include <QObject>
#include <memory>

//------------------------------------------------------------------------------
class Backend
  : public QObject
  , public eagine::main_ctx_object {
    Q_OBJECT

    Q_PROPERTY(Theme* theme READ getTheme CONSTANT)
public:
    Backend(eagine::main_ctx_parent);

    auto getTheme() noexcept -> Theme*;
signals:
public slots:

private:
    Theme _theme;
};
//------------------------------------------------------------------------------
#endif
