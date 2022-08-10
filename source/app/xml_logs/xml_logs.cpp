///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///

import eagine.core;
#include "Backend.hpp"
#include "internal_log_backend.hpp"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
namespace eagine {
//------------------------------------------------------------------------------
static int argc_copy = 0;
static const char** argv_copy = nullptr;
//------------------------------------------------------------------------------
auto main(main_ctx& ctx) -> int {
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app{argc_copy, const_cast<char**>(argv_copy)};
    app.setOrganizationName("EAGine");
    app.setOrganizationDomain("oglplus.org");
    app.setApplicationName("XML log viewer");

    const auto registerId = "com.github.matus-chochlik.eagine.core.xml_logs";

    qmlRegisterUncreatableType<Theme>(registerId, 1, 0, "Theme", {});
    qmlRegisterUncreatableType<EntryLog>(registerId, 1, 0, "EntryLog", {});
    qmlRegisterUncreatableType<EntryViewModel>(
      registerId, 1, 0, "EntryViewModel", {});

    Backend backend{ctx};
    backend.entryLog().assignStorage(internal_log_backend::storage());
    backend.entryLog().assignStorage(std::make_shared<ActivityStorage>());
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
    options.app_id = "XmlLogView";
    options.logger_opts.forced_backend =
      std::make_unique<eagine::internal_log_backend>();
    return eagine::main_impl(argc, argv, options, eagine::main);
}
