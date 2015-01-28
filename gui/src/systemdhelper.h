#ifndef SYSTEMDHELPER_H
#define SYSTEMDHELPER_H

#include <QObject>
#include <QDBusConnection>
#include <QDBusInterface>

class SystemdHelper : public QObject
{
    Q_OBJECT
public:
    explicit SystemdHelper(QObject *parent = 0);
    Q_PROPERTY(bool serviceEnabled READ serviceEnabled NOTIFY serviceEnabledChanged)
    bool serviceEnabled() const;

    Q_INVOKABLE bool checkEnabled();
    Q_INVOKABLE void setEnabled(bool enabled);
    Q_INVOKABLE void forceStart();
    Q_INVOKABLE void forceExit();

private:
    QScopedPointer<QDBusInterface> daemonIface;

    bool m_serviceEnabled;

signals:
    void serviceEnabledChanged();

};

#endif // SYSTEMDHELPER_H
