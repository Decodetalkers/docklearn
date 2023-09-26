#include "backgrounds/x11/XorgBackground.h"
#include "models/elementGroupModel.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>

void
registerGlobalType()
{
    qmlRegisterSingletonType<ElementGroupModel>(
      "Dock.Global", 1, 0, "ElementGroupModel", [](QQmlEngine *, QJSEngine *) {
          return new ElementGroupModel;
      });
}

int
main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    registerGlobalType();
    QQmlApplicationEngine engine;

    const QUrl url = QUrl("qrc:/deepin/dock/main.qml");
    QObject::connect(
      &engine,
      &QQmlApplicationEngine::objectCreated,
      &app,
      [url](QObject *obj, const QUrl &objUrl) {
          if (!obj && url == objUrl) {
              XorgBackground::instance()->stoploop();
              XorgBackground::instance()->quit();
              XorgBackground::instance()->wait();
              QCoreApplication::exit(-1);
          }
      },
      Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
