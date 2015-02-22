#ifndef VOICEMAILWATCHER_H
#define VOICEMAILWATCHER_H

#include <QObject>

#include <qofonomanager.h>
#include <qofonomessagewaiting.h>

#include <QDBusConnection>
#include <QDBusInterface>

#include <MGConfItem>

#include <QGuiApplication>
#include <QQuickView>

#include "sailfishapp.h"

#include "notification.h"

class VoicemailWatcher : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.coderus.voicemailwatcher")
public:
    explicit VoicemailWatcher(QObject *parent = 0);

signals:

public slots:
    void start();

    Q_SCRIPTABLE Q_NOREPLY void showUI(const QStringList &args);
    Q_SCRIPTABLE Q_NOREPLY void notificationCallback();
    Q_SCRIPTABLE Q_NOREPLY void clearAndExit();
    Q_SCRIPTABLE Q_NOREPLY void testVoicemailWaitingChanged(bool isWaiting);

private slots:
    void onVoicemailWaitingChanged(bool isWaiting);
    void onVoicemailMessageCountChanged(int count);
    void onVoicemailMailboxNumberChanged(const QString &number);

    void onViewDestroyed();
    void onViewClosing(QQuickCloseEvent*);

    void onModemsChanged(const QStringList &modems);
    void onMessageWaitingReadyChanged();

private:
    void selectModem(const QString &modemPath);
    void connectOfonoVoicemail();

    void clearNotification();

    QDBusInterface *voiceCallManagerIface;
    QOfonoMessageWaiting *messageWaiting;
    MGConfItem *dconf;
    QQuickView *view;
};

#endif // VOICEMAILWATCHER_H
