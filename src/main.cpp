#include <sailfishapp.h>
#include <QGuiApplication>
#include <QTimer>

#include "voicemailwatcher.h"

int main(int argc, char *argv[])
{
    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    app->setApplicationDisplayName("Voicemail notifications");
    app->setApplicationName("Voicemail notifications");
    app->setApplicationVersion(QString(APP_VERSION));
    app->setQuitOnLastWindowClosed(false);
    QScopedPointer<VoicemailWatcher> watcher(new VoicemailWatcher(app.data()));
    QTimer::singleShot(10, watcher.data(), SLOT(start()));

    return app->exec();
}

