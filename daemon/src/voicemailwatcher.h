#ifndef VOICEMAILWATCHER_H
#define VOICEMAILWATCHER_H

#include <QObject>
#include <QDBusConnection>
#include <QDBusInterface>
#include <qofonomanager.h>
#include <qofonomessagewaiting.h>
#include <MGConfItem>

#include "notification.h"

class VoicemailWatcher : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.coderus.voicemailwatcher")
public:
    explicit VoicemailWatcher(QObject *parent = 0);
    bool getReady() const;

public slots:
    Q_SCRIPTABLE void notificationCallback();
    Q_SCRIPTABLE void ping();
    Q_SCRIPTABLE void exit();

private slots:
    void notificationClicked();
    void notificationClosed(uint reason);

    void onVoicemailWaitingChanged(bool isWaiting);
    void onVoicemailMessageCountChanged(int count);
    void onVoicemailMailboxNumberChanged(const QString &number);

private:
    QScopedPointer<Notification> oldNotification;
    QScopedPointer<QOfonoMessageWaiting> messageWaiting;
    QScopedPointer<QDBusInterface> voiceCallManagerIface;
    QScopedPointer<MGConfItem> dconf;

    bool ready;

};

#endif // VOICEMAILWATCHER_H
