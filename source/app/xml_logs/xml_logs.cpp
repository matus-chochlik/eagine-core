///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

#include "Backend.hpp"
#include <eagine/main_ctx.hpp>
#include <eagine/main_fwd.hpp>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
namespace eagine {
//------------------------------------------------------------------------------
int argc_copy = 0;
const char** argv_copy = nullptr;
//------------------------------------------------------------------------------
auto main(main_ctx& ctx) -> int {
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app{argc_copy, const_cast<char**>(argv_copy)};
    app.setOrganizationName("EAGine");
    app.setOrganizationDomain("oglplus.org");
    app.setApplicationName("XML log viewer");

    const auto registerId = "com.github.matus-chochlik.eagine.core.xml_logs";

    qmlRegisterUncreatableType<Theme>(registerId, 1, 0, "Theme", {});

    Backend backend{ctx};
    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("backend", &backend);
    engine.load("qrc:///xml_logs.qml");

    return app.exec();
}
//------------------------------------------------------------------------------
} // namespace eagine

auto main(int argc, const char** argv) -> int {
    eagine::argc_copy = argc;
    eagine::argv_copy = argv;
    eagine::main_ctx_options options;
    options.app_id = EAGINE_ID(XmlLogView);
    return eagine::main_impl(argc, argv, options);
}

