#include <QFile>
#include <QDir>

#include "systemdhelper.h"

#define AUTOSTART_DIR "%1/.config/systemd/user/post-user-session.target.wants"
#define AUTOSTART_USER "%1/.config/systemd/user/post-user-session.target.wants/voicemail-daemon.service"
#define AUTOSTART_SERVICE "/usr/lib/systemd/user/voicemail-daemon.service"

SystemdHelper::SystemdHelper(QObject *parent) :
    QObject(parent)
{
    daemonIface.reset(new QDBusInterface("org.coderus.voicemailwatcher", "/", "org.coderus.voicemailwatcher",
                                         QDBusConnection::sessionBus(), this));

    m_serviceEnabled = checkEnabled();
    Q_EMIT serviceEnabledChanged();
}

bool SystemdHelper::serviceEnabled() const
{
    return m_serviceEnabled;
}

bool SystemdHelper::checkEnabled()
{
    QString autostartUser = QString(AUTOSTART_USER).arg(QDir::homePath());
    QFile service(autostartUser);
    return service.exists();
}

void SystemdHelper::setEnabled(bool enabled)
{
    QString autostartUser = QString(AUTOSTART_USER).arg(QDir::homePath());
    if (enabled) {
        QString autostartDir = QString(AUTOSTART_DIR).arg(QDir::homePath());
        QDir dir(autostartDir);
        if (!dir.exists())
            dir.mkpath(autostartDir);
        QFile service(AUTOSTART_SERVICE);
        service.link(autostartUser);
    }
    else {
        QFile service(autostartUser);
        service.remove();
    }

    m_serviceEnabled = enabled;
    Q_EMIT serviceEnabledChanged();
}

void SystemdHelper::forceStart()
{
    if (daemonIface) {
        daemonIface->call(QDBus::NoBlock, "ping");
    }
}

void SystemdHelper::forceExit()
{
    if (daemonIface) {
        daemonIface->call(QDBus::NoBlock, "exit");
    }
}
