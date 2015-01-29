#include "voicemailwatcher.h"
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

    messageWaiting.reset(new QOfonoMessageWaiting(this));

    QOfonoManager* manager = new QOfonoManager(this);
    QStringList modems = manager->modems();
    qDebug() << "Available modems:" << modems;

    if (modems.size() > 0) {
        qDebug() << "Selecting modem:" << modems.first();
        messageWaiting->setModemPath(modems.first());

        voiceCallManagerIface.reset(new QDBusInterface("org.ofono", modems.first(), "org.ofono.VoiceCallManager",
                                                       QDBusConnection::systemBus(), this));

        dconf.reset(new MGConfItem("/apps/voicemail-notifications/id", this));

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

    Notification *startup = new Notification(this);
    startup->setCategory("custom");
    startup->setSummary("Voicemail watcher ready");
    startup->setBody("Just to let you know");
    startup->setPreviewSummary(startup->summary());
    startup->setPreviewBody(startup->body());
    startup->setItemCount(1);
    startup->publish();
}

bool VoicemailWatcher::getReady() const
{
    return ready;
}

void VoicemailWatcher::notificationCallback()
{
    qDebug() << "notificationCallback";
    if (voiceCallManagerIface && messageWaiting) {
        voiceCallManagerIface->call(QDBus::NoBlock, "Dial", messageWaiting->voicemailMailboxNumber(), QString());
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

    Notification *shutdown = new Notification(this);
    shutdown->setCategory("custom");
    shutdown->setSummary("Voicemail watcher exiting");
    shutdown->setBody("Remote application requested exit");
    shutdown->setPreviewSummary(shutdown->summary());
    shutdown->setPreviewBody(shutdown->body());
    shutdown->setItemCount(1);
    shutdown->publish();

    QCoreApplication::quit();
}

void VoicemailWatcher::notificationClicked()
{
    Notification *notification = qobject_cast<Notification*>(sender());
    if (notification) {
        qDebug() << "notificationClicked" << notification->summary();
    }
}

void VoicemailWatcher::notificationClosed(uint reason)
{
    Notification *notification = qobject_cast<Notification*>(sender());
    if (notification) {
        qDebug() << "notificationClosed" << notification->summary() << notification->replacesId() << reason;
        if (notification->replacesId() == dconf->value(0).toUInt() && notification->replacesId() != 0) {
            dconf->set(0);
        }
    }

    onVoicemailWaitingChanged(messageWaiting->voicemailWaiting());
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

        Notification *notification;
        if (oldNotification) {
            notification = oldNotification.take();
            notification->setBody(body);
            notification->setPreviewBody(body);
        }
        else {
            notification = new Notification(this);
            notification->setCategory("x-nemo.messaging.voicemail");
            notification->setSummary(summary);
            notification->setBody(body);
            notification->setPreviewSummary(summary);
            notification->setPreviewBody(body);
            notification->setItemCount(1);
            QVariantList remoteActions;
            QVariantMap action1;
            action1["name"] = "call";
            action1["displayName"] = "Call to mailbox";
            action1["icon"] = "icon-lock-voicemail";
            action1["service"] = "org.coderus.voicemailwatcher";
            action1["path"] = "/";
            action1["iface"] = "org.coderus.voicemailwatcher";
            action1["method"] = "notificationCallback";
            remoteActions.append(action1);
            notification->setRemoteActions(remoteActions);
            QObject::connect(notification, SIGNAL(closed(uint)), this, SLOT(notificationClosed(uint)));

            uint id = dconf->value(0).toUInt();
            if (id > 0) {
                notification->setReplacesId(id);
            }

            notification->publish();

            dconf->set(notification->replacesId());
        }
        notification->publish();
        oldNotification.reset(notification);
    }
    else {
        if (oldNotification && oldNotification->replacesId() > 0) {
            oldNotification->close();
        }
        oldNotification.reset();
        dconf->set(0);
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
