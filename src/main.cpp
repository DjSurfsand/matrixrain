// matrixrain — rains your wallpaper's characters down the screen, on demand.

#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQuickStyle>
#include <QQuickWindow>
#include <QUrl>

#include <LayerShellQt/Window>

#include "backend.h"
#include "theme.h"
#include "translator.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    app.setApplicationName("matrixrain");
    app.setApplicationVersion("0.1.0");

    // Matches the window to matrixrain.desktop, so the compositor (Wayland
    // app_id) and the launcher pick up the installed icon.
    app.setDesktopFileName("matrixrain");
    app.setWindowIcon(QIcon::fromTheme("matrixrain"));

    QQuickStyle::setStyle("Material");

    // Installed before anything is translated, and every qsTr() binding is
    // re-evaluated when the Omarchy language changes.
    Translator translator;
    app.installTranslator(&translator);

    Theme theme;
    Backend backend;

    QQmlApplicationEngine engine;
    QObject::connect(&translator, &Translator::changed, &engine, &QQmlEngine::retranslate);
    engine.rootContext()->setContextProperty("translator", &translator);
    engine.rootContext()->setContextProperty("theme", &theme);
    engine.rootContext()->setContextProperty("backend", &backend);

    engine.load(QUrl("qrc:/Main.qml"));
    if (engine.rootObjects().isEmpty())
        return 1;

    // The rain lives on its own wlr-layer-shell surface: anchored to every
    // edge, on the bottom layer so it sits above the wallpaper and below the
    // user's windows, and with no keyboard interactivity so it never steals
    // input. It is created hidden and shown only while the effect is on.
    QQmlComponent overlayComponent(&engine, QUrl("qrc:/RainWindow.qml"));
    if (!overlayComponent.isReady()) {
        qWarning() << "Rain overlay failed to load:" << overlayComponent.errorString();
        return 1;
    }
    QObject *overlayObject = overlayComponent.create();
    auto *overlay = qobject_cast<QQuickWindow *>(overlayObject);
    if (!overlay) {
        qWarning() << "Rain overlay is not a window";
        return 1;
    }

    auto *layer = LayerShellQt::Window::get(overlay);
    layer->setScope("matrixrain");
    layer->setLayer(LayerShellQt::Window::LayerBottom);
    LayerShellQt::Window::Anchors allEdges;
    allEdges |= LayerShellQt::Window::AnchorTop;
    allEdges |= LayerShellQt::Window::AnchorBottom;
    allEdges |= LayerShellQt::Window::AnchorLeft;
    allEdges |= LayerShellQt::Window::AnchorRight;
    layer->setAnchors(allEdges);
    layer->setExclusiveZone(0);
    layer->setKeyboardInteractivity(LayerShellQt::Window::KeyboardInteractivityNone);
    layer->setCloseOnDismissed(false);

    QObject::connect(&backend, &Backend::enabledChanged, &app, [overlay, &backend] {
        overlay->setVisible(backend.enabled());
    });
    overlay->setVisible(backend.enabled());

    return app.exec();
}
