import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import MatrixRain

// A transparent, full-screen layer-shell surface. C++ sets the layer, anchors
// and keyboard policy before showing it, so QML only owns the visuals.
ApplicationWindow {
    id: overlay
    visible: false
    color: "transparent"
    flags: Qt.FramelessWindowHint

    Material.theme: Material.Dark
    Material.accent: theme.accent

    RainItem {
        anchors.fill: parent
        cell: 24 - backend.density
        speed: backend.speed
        glyphs: backend.glyphs
        accent: theme.accent
        running: overlay.visible && backend.enabled
    }
}
