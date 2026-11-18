import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

ApplicationWindow {
    id: win
    width: 420
    height: 470
    minimumWidth: 380
    minimumHeight: 430
    visible: true
    title: qsTr("Matrix Rain")
    color: "#07100a"

    Material.theme: Material.Dark
    Material.accent: theme.accent
    Material.roundedScale: theme.radius

    property bool helpVisible: false

    Shortcut { sequence: "?"; context: Qt.ApplicationShortcut; onActivated: win.helpVisible = !win.helpVisible }
    Shortcut { sequence: "Q"; context: Qt.ApplicationShortcut; onActivated: Qt.quit() }
    Shortcut { sequence: "Space"; context: Qt.ApplicationShortcut; onActivated: backend.toggle() }

    // Right-to-left languages mirror the window; the Translator sets the
    // application's layout direction.
    Item {
        anchors.fill: parent
        LayoutMirroring.enabled: Qt.application.layoutDirection === Qt.RightToLeft
        LayoutMirroring.childrenInherit: true

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 24
            spacing: 20

            RowLayout {
                spacing: 14
                Rectangle {
                    Layout.preferredWidth: 44
                    Layout.preferredHeight: 44
                    radius: Math.min(10, theme.radius)
                    color: theme.accent
                    Label {
                        anchors.centerIn: parent
                        text: "ア"
                        font.pixelSize: 26
                        font.bold: true
                        color: "#001a08"
                    }
                }
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 2
                    Label {
                        text: qsTr("Matrix Rain")
                        font.pixelSize: 22
                        font.bold: true
                        color: theme.accent
                    }
                    Label {
                        text: qsTr("Bring your wallpaper to life")
                        font.pixelSize: 12
                        color: "#79977f"
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 88
                radius: Math.min(10, theme.radius)
                color: "#0c1a10"
                border.width: 1
                border.color: backend.enabled ? theme.accent : "#1d3524"
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 12
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 3
                        Label {
                            text: qsTr("Digital rain")
                            font.pixelSize: 16
                            font.bold: true
                        }
                        Label {
                            text: backend.enabled ? qsTr("Falling over your wallpaper") : qsTr("Paused")
                            font.pixelSize: 12
                            color: "#79977f"
                        }
                    }
                    Switch {
                        checked: backend.enabled
                        onToggled: backend.setEnabled(checked)
                    }
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 2
                RowLayout {
                    Layout.fillWidth: true
                    Label { text: qsTr("Speed"); font.pixelSize: 13 }
                    Item { Layout.fillWidth: true }
                    Label { text: backend.speed; color: theme.accent; font.pixelSize: 13; font.bold: true }
                }
                Slider {
                    Layout.fillWidth: true
                    from: 1
                    to: 10
                    stepSize: 1
                    value: backend.speed
                    onMoved: backend.setSpeed(Math.round(value))
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 2
                RowLayout {
                    Layout.fillWidth: true
                    Label { text: qsTr("Density"); font.pixelSize: 13 }
                    Item { Layout.fillWidth: true }
                    Label { text: backend.density; color: theme.accent; font.pixelSize: 13; font.bold: true }
                }
                Slider {
                    Layout.fillWidth: true
                    from: 1
                    to: 10
                    stepSize: 1
                    value: backend.density
                    onMoved: backend.setDensity(Math.round(value))
                }
            }

            Item { Layout.fillHeight: true }

            RowLayout {
                Layout.fillWidth: true
                Label {
                    text: qsTr("Space toggle · Q quit · ? help")
                    font.pixelSize: 11
                    color: "#5c7a66"
                    Layout.fillWidth: true
                }
                Button {
                    text: qsTr("Quit")
                    flat: true
                    onClicked: Qt.quit()
                }
            }
        }

        // Shortcut overlay, dismissed with ? again or a click.
        Rectangle {
            anchors.fill: parent
            visible: win.helpVisible
            color: "#030a06"
            opacity: 0.97
            z: 10
            MouseArea {
                anchors.fill: parent
                onClicked: win.helpVisible = false
            }
            ColumnLayout {
                anchors.centerIn: parent
                spacing: 12
                width: parent.width - 48
                Label {
                    Layout.alignment: Qt.AlignHCenter
                    text: qsTr("Shortcuts")
                    font.pixelSize: 20
                    font.bold: true
                    color: theme.accent
                }
                Label {
                    Layout.alignment: Qt.AlignHCenter
                    text: qsTr("Space — toggle the rain")
                    color: "#b9d3c0"
                }
                Label {
                    Layout.alignment: Qt.AlignHCenter
                    text: qsTr("Q — quit")
                    color: "#b9d3c0"
                }
                Label {
                    Layout.alignment: Qt.AlignHCenter
                    text: qsTr("? — show or hide this help")
                    color: "#b9d3c0"
                }
                Label {
                    Layout.alignment: Qt.AlignHCenter
                    text: qsTr("Click anywhere to close")
                    font.pixelSize: 11
                    color: "#5c7a66"
                }
            }
        }
    }
}
