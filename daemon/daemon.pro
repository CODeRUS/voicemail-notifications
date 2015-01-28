TARGET = voicemail-daemon
target.path = /usr/bin

QT += dbus
CONFIG += link_pkgconfig qofono-qt5
PKGCONFIG += mlite5

INCLUDEPATH += /usr/include/mlite5

SOURCES += \
    src/main.cpp \
    src/voicemailwatcher.cpp

HEADERS += \
    src/voicemailwatcher.h

dbus.files = dbus/org.coderus.voicemailwatcher.service
dbus.path = /usr/share/dbus-1/services

systemd.files = systemd/voicemail-daemon.service
systemd.path = /usr/lib/systemd/user

INSTALLS = target systemd dbus

