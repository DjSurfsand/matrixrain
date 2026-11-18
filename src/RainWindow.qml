import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

// A transparent, full-screen layer-shell surface. C++ sets the layer, anchors
// and keyboard policy before showing it, so QML only owns the visuals.
ApplicationWindow {
    id: overlay
    visible: false
    color: "transparent"
    flags: Qt.FramelessWindowHint

    Material.theme: Material.Dark
    Material.accent: theme.accent

    RainCanvas {
        anchors.fill: parent
    }
}
