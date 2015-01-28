#include "voicemailwatcher.h"
#include <MRemoteAction>

VoicemailWatcher::VoicemailWatcher(QObject *parent) :
    QObject(parent),
    ready(false)
{
    ready =
        QDBusConnection::sessionBus().registerService("org.coderus.voicemailwatcher") &&
        QDBusConnection::sessionBus().registerObject("/", this, QDBusConnection::ExportScriptableSlots);

    if (!ready) {
        QCoreApplication::quit();
        return;
    }

    voiceCallManagerIface.reset(new QDBusInterface("org.ofono", "/", "org.ofono.VoiceCallManager",
                                                   QDBusConnection::systemBus(), this));

    messageWaiting.reset(new QOfonoMessageWaiting(this));

    QOfonoManager* manager = new QOfonoManager(this);
    QStringList modems = manager->modems();

    if (modems.size() > 0) {
        messageWaiting->setModemPath(modems.first());

        if (messageWaiting->isValid()) {
            QObject::connect(messageWaiting.data(), SIGNAL(voicemailWaitingChanged(bool)), this, SLOT(onVoicemailWaitingChanged(bool)));
            QObject::connect(messageWaiting.data(), SIGNAL(voicemailMessageCountChanged(int)), this, SLOT(onVoicemailMessageCountChanged(int)));
            QObject::connect(messageWaiting.data(), SIGNAL(voicemailMailboxNumberChanged(QString)), this, SLOT(onVoicemailMailboxNumberChanged(QString)));

            onVoicemailMessageCountChanged(messageWaiting->voicemailMessageCount());
        }
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
    if (voiceCallManagerIface && messageWaiting) {
        voiceCallManagerIface->call("Dial", messageWaiting->voicemailMailboxNumber(), QString());
    }
}

void VoicemailWatcher::ping()
{
    return;
}

void VoicemailWatcher::exit()
{
    MNotification shutdown("custom", "Voicemail watcher exiting", "Remote application requested exit");
    shutdown.publish();

    QCoreApplication::quit();
}

void VoicemailWatcher::onVoicemailWaitingChanged(bool isWaiting)
{
    if (!isWaiting && oldNotification) {
        if (oldNotification->isPublished()) {
            oldNotification->remove();
        }
        oldNotification.reset();
    }
}

void VoicemailWatcher::onVoicemailMessageCountChanged(int count)
{
    if (count > 0) {
        QString summary("New voicemail");
        QString body = QString("You have %1 voicemail").arg(count);

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
}

void VoicemailWatcher::onVoicemailMailboxNumberChanged(const QString &number)
{
    Q_UNUSED(number)
    // do nothing yet
}
