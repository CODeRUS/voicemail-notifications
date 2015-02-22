#include "voicemailwatcher.h"

#include <QDebug>

VoicemailWatcher::VoicemailWatcher(QObject *parent) :
    QObject(parent)
{
    view = NULL;
    dconf = NULL;
    voiceCallManagerIface = NULL;
    messageWaiting = NULL;
}

void VoicemailWatcher::start()
{
    bool ready = QDBusConnection::sessionBus().registerService("org.coderus.voicemailwatcher");

    if (!ready) {
        qWarning() << "Service already registered, exiting...";
        QGuiApplication::quit();
        return;
    }
    else {
        qDebug() << "Service registered successfully!";

        QOfonoManager* manager = new QOfonoManager(this);

        QStringList modems = manager->modems();

        if (modems.size() > 0) {
            selectModem(modems.first());
        }
        else {
            qWarning() << "Waiting for modems from manager...";
            QObject::connect(manager, SIGNAL(modemsChanged(QStringList)), this, SLOT(onModemsChanged(QStringList)));
        }
    }
}

void VoicemailWatcher::showUI(const QStringList &args)
{
    Q_UNUSED(args)
    if (!view) {
        qDebug() << "Construct view";
        view = SailfishApp::createView();
        QObject::connect(view, SIGNAL(destroyed()), this, SLOT(onViewDestroyed()));
        QObject::connect(view, SIGNAL(closing(QQuickCloseEvent*)), this, SLOT(onViewClosing(QQuickCloseEvent*)));
        view->setTitle("Voicemail notifications");
        view->setSource(SailfishApp::pathTo("qml/main.qml"));
        view->showFullScreen();
    }
    else if (view->windowState() == Qt::WindowNoState) {
        qDebug() << "Create view";
        view->create();
        view->showFullScreen();
    }
    else {
        qDebug() << "Show view";
        view->raise();
        view->requestActivate();
    }

}

void VoicemailWatcher::notificationCallback()
{
    if (messageWaiting && messageWaiting->isReady()) {
        qDebug() << "calling to" << messageWaiting->voicemailMailboxNumber();
        if (!voiceCallManagerIface) {
            voiceCallManagerIface = new QDBusInterface("org.ofono", messageWaiting->modemPath(), "org.ofono.VoiceCallManager",
                                                       QDBusConnection::systemBus(), this);
        }
        voiceCallManagerIface->call(QDBus::NoBlock, "Dial", messageWaiting->voicemailMailboxNumber(), QString());
    }
}

void VoicemailWatcher::clearAndExit()
{
    qDebug() << "Exit requested";
    clearNotification();
    QGuiApplication::quit();
}

void VoicemailWatcher::testVoicemailWaitingChanged(bool isWaiting)
{
    qDebug() << isWaiting;
    onVoicemailWaitingChanged(isWaiting);
}

void VoicemailWatcher::onVoicemailWaitingChanged(bool isWaiting)
{
    qDebug() << isWaiting;

    if (isWaiting) {
        Notification *notification = new Notification(this);
        QString summary("New voicemail");
        QString body("You have new voicemail");
        int count = messageWaiting->voicemailMessageCount();
        if (count > 1) {
            body = QString("You have new voicemails");
        }
        else {
            count = 1;
        }
        notification->setCategory("org.coderus.voicemailnotification");
        notification->setSummary(summary);
        notification->setBody(body);
        notification->setPreviewSummary(summary);
        notification->setPreviewBody(body);
        notification->setItemCount(count);
        QVariantList remoteActions;
        QVariantMap action1;
        action1["name"] = "default";
        action1["displayName"] = "Call to mailbox";
        action1["icon"] = "icon-lock-voicemail";
        action1["service"] = "org.coderus.voicemailwatcher";
        action1["path"] = "/";
        action1["iface"] = "org.coderus.voicemailwatcher";
        action1["method"] = "notificationCallback";
        remoteActions.append(action1);
        notification->setRemoteActions(remoteActions);
        notification->setReplacesId(dconf->value(0).toUInt());
        notification->publish();

        qDebug() << "New notification" << notification->replacesId();
        dconf->set(notification->replacesId());
    }
    else {
        clearNotification();
    }
}

void VoicemailWatcher::onVoicemailMessageCountChanged(int count)
{
    qDebug() << count;
    // do nothing yet
}

void VoicemailWatcher::onVoicemailMailboxNumberChanged(const QString &number)
{
    qDebug() << number;
    // do nothing yet
}

void VoicemailWatcher::onViewDestroyed()
{
    qDebug() << "Window destroyed";
    view = NULL;
}

void VoicemailWatcher::onViewClosing(QQuickCloseEvent*)
{
    qDebug() << "Window closed";
    delete view;
}

void VoicemailWatcher::onModemsChanged(const QStringList &modems)
{
    qDebug() << modems;
    QObject::disconnect(sender(), 0, 0, 0);
    selectModem(modems.first());
}

void VoicemailWatcher::onMessageWaitingReadyChanged()
{
    qDebug() << "Ready:" << messageWaiting->isReady();
    if (messageWaiting->isReady()) {
        QObject::disconnect(sender(), 0, 0, 0);
        connectOfonoVoicemail();
    }
    else {
        qWarning() << "Not ready!";
    }
}

void VoicemailWatcher::selectModem(const QString &modemPath)
{
    qDebug() << modemPath;

    messageWaiting = new QOfonoMessageWaiting(this);
    messageWaiting->setModemPath(modemPath);

    if (messageWaiting->isReady()) {
        connectOfonoVoicemail();
    }
    else {
        qDebug() << "Waiting for QOfonoMessageWaiting to be ready...";
        QObject::connect(messageWaiting, SIGNAL(readyChanged()), this, SLOT(onMessageWaitingReadyChanged()));
    }
}

void VoicemailWatcher::connectOfonoVoicemail()
{
    qDebug() << "Connecting to signals";

    dconf = new MGConfItem("/apps/voicemail-notifications/id", this);

    QDBusConnection::sessionBus().registerObject("/", this, QDBusConnection::ExportScriptableSlots);

    QObject::connect(messageWaiting, SIGNAL(voicemailWaitingChanged(bool)), this, SLOT(onVoicemailWaitingChanged(bool)));
    QObject::connect(messageWaiting, SIGNAL(voicemailMessageCountChanged(int)), this, SLOT(onVoicemailMessageCountChanged(int)));
    QObject::connect(messageWaiting, SIGNAL(voicemailMailboxNumberChanged(QString)), this, SLOT(onVoicemailMailboxNumberChanged(QString)));

    qDebug() << "Voicemail waiting: " << messageWaiting->voicemailWaiting();
    qDebug() << "Voicemail count: " << messageWaiting->voicemailMessageCount();
    qDebug() << "Voicemail number: " << messageWaiting->voicemailMailboxNumber();

    onVoicemailWaitingChanged(messageWaiting->voicemailWaiting());
}

void VoicemailWatcher::clearNotification()
{
    if (dconf->value(0).toUInt() > 0) {
        Notification *notification = new Notification(this);
        notification->setReplacesId(dconf->value(0).toUInt());
        notification->close();
        dconf->set(0);
    }
}
