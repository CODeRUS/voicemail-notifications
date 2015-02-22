TARGET = voicemail-notifications

QT += dbus
CONFIG += link_pkgconfig qofono-qt5 sailfishapp
PKGCONFIG += mlite5 sailfishapp

INCLUDEPATH += /usr/include/mlite5

DEFINES += APP_VERSION=\\\"$$VERSION\\\"

SOURCES += \
    src/main.cpp \
    src/notificationmanagerproxy.cpp \
    src/notification.cpp \
    src/voicemailwatcher.cpp

HEADERS += \
    src/notificationmanagerproxy.h \
    src/notification.h \
    src/voicemailwatcher.h

dbus.files = dbus/org.coderus.voicemailwatcher.service
dbus.path = /usr/share/dbus-1/services

systemd.files = systemd/voicemail-notifications.service
systemd.path = /usr/lib/systemd/user

notification.files = notification/org.coderus.voicemailnotification.conf
notification.path = /usr/share/lipstick/notificationcategories

INSTALLS += dbus systemd notification

OTHER_FILES += \
    qml/cover/CoverPage.qml \
    qml/pages/MainPage.qml \
    qml/main.qml \
    rpm/voicemail-notifications.spec \
    voicemail-notifications.desktop
