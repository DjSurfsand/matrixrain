QT += core gui qml quick quickcontrols2 dbus

CONFIG += c++17 release
TARGET = matrixrain
TEMPLATE = app

INCLUDEPATH += src

HEADERS += \
    src/backend.h \
    src/theme.h \
    src/translator.h

SOURCES += \
    src/main.cpp \
    src/backend.cpp \
    src/theme.cpp \
    src/translator.cpp

# LayerShellQt puts the rain window on a real wlr-layer-shell surface so it
# lives above the wallpaper and never takes focus from the user's windows.
LIBS += -lLayerShellQtInterface

RESOURCES += src/resources.qrc
