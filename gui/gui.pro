TARGET = voicemail-notifications

QT += dbus
CONFIG += sailfishapp

SOURCES += \
    src/main.cpp \
    src/systemdhelper.cpp

HEADERS += \
    src/systemdhelper.h

OTHER_FILES += qml/voicemail-notifications.qml \
    qml/pages/FirstPage.qml \
    qml/pages/SecondPage.qml \
    qml/cover/CoverPage.qml \
    voicemail-notifications.desktop \
    voicemail-notifications.png
