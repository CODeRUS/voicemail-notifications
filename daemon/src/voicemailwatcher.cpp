#include "voicemailwatcher.h"
#include <MRemoteAction>
#include <QDebug>

VoicemailWatcher::VoicemailWatcher(QObject *parent) :
    QObject(parent),
    ready(false)
{
    ready =
        QDBusConnection::sessionBus().registerService("org.coderus.voicemailwatcher") &&
        QDBusConnection::sessionBus().registerObject("/", this, QDBusConnection::ExportScriptableSlots);

    if (!ready) {
        qWarning() << "Bus already registered, exiting...";
        QCoreApplication::quit();
        return;
    }
    else {
        qDebug() << "Bus registered successfully!";
    }

    voiceCallManagerIface.reset(new QDBusInterface("org.ofono", "/", "org.ofono.VoiceCallManager",
                                                   QDBusConnection::systemBus(), this));

    messageWaiting.reset(new QOfonoMessageWaiting(this));

    QOfonoManager* manager = new QOfonoManager(this);
    QStringList modems = manager->modems();
    qDebug() << "Available modems:" << modems;

    if (modems.size() > 0) {
        qDebug() << "Selecting modem:" << modems.first();
        messageWaiting->setModemPath(modems.first());

        if (messageWaiting->isValid()) {
            qDebug() << "Connecting to signals";
            QObject::connect(messageWaiting.data(), SIGNAL(voicemailWaitingChanged(bool)), this, SLOT(onVoicemailWaitingChanged(bool)));
            QObject::connect(messageWaiting.data(), SIGNAL(voicemailMessageCountChanged(int)), this, SLOT(onVoicemailMessageCountChanged(int)));
            QObject::connect(messageWaiting.data(), SIGNAL(voicemailMailboxNumberChanged(QString)), this, SLOT(onVoicemailMailboxNumberChanged(QString)));

            qDebug() << "Object created successfully: " << messageWaiting->isReady();
            qDebug() << "Voicemail waiting: " << messageWaiting->voicemailWaiting();
            qDebug() << "Voicemail count: " << messageWaiting->voicemailMessageCount();
            qDebug() << "Voicemail number: " << messageWaiting->voicemailMailboxNumber();

            onVoicemailWaitingChanged(messageWaiting->voicemailWaiting());
        }
        else {
            qWarning() << "Object is not valid, exiting!";
            ready = false;
            return;
        }
    }
    else {
        qWarning() << "No modems found, exiting!";
        ready = false;
        return;
    }

    MNotification startup("custom", "Voicemail watcher ready", "Just to let you know");
    startup.publish();

}

bool VoicemailWatcher::getReady() const
{
    return ready;
}

void VoicemailWatcher::notificationCallback()
{
    qDebug() << "notificationCallback";
    if (voiceCallManagerIface && messageWaiting) {
        voiceCallManagerIface->call("Dial", messageWaiting->voicemailMailboxNumber(), QString());
    }
}

void VoicemailWatcher::ping()
{
    qDebug() << "Ping received";
    return;
}

void VoicemailWatcher::exit()
{
    qDebug() << "Exit requested";
    MNotification shutdown("custom", "Voicemail watcher exiting", "Remote application requested exit");
    shutdown.publish();

    QCoreApplication::quit();
}

void VoicemailWatcher::onVoicemailWaitingChanged(bool isWaiting)
{
    qDebug() << "onVoicemailWaitingChanged: " << isWaiting;

    if (isWaiting) {
        QString summary("New voicemail");
        QString body("You have new voicemail");
        int count = messageWaiting->voicemailMessageCount();
        if (count > 1) {
            body = QString("You have %1 voicemails").arg(count);
        }

        MNotification *notification;
        if (oldNotification) {
            notification = oldNotification.take();
            notification->setBody(body);
        }
        else {
            notification = new MNotification("x-nemo.messaging.voicemail", summary, body);
            notification->setImage("icon-lock-voicemail");
            MRemoteAction action("org.coderus.voicemailwatcher", "/", "org.coderus.voicemailwatcher", "notificationCallback");
            notification->setAction(action);
        }
        notification->publish();
        oldNotification.reset(notification);
    }
    else {
        if (oldNotification && oldNotification->isPublished()) {
            oldNotification->remove();
        }
        oldNotification.reset();
    }
}

void VoicemailWatcher::onVoicemailMessageCountChanged(int count)
{
    qDebug() << "onVoicemailMessageCountChanged: " << count;
    // do nothing yet
}

void VoicemailWatcher::onVoicemailMailboxNumberChanged(const QString &number)
{
    qDebug() << "onVoicemailMailboxNumberChanged: " << number;
    // do nothing yet
}
