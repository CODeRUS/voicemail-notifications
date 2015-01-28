#include <QCoreApplication>
#include <QScopedPointer>
#include "voicemailwatcher.h"

int main(int argc, char *argv[])
{
    QScopedPointer<QCoreApplication> app(new QCoreApplication(argc, argv));
    QScopedPointer<VoicemailWatcher> watcher(new VoicemailWatcher(app.data()));
    if (watcher->getReady()) {
        return app->exec();
    }
    else {
        return 0;
    }
}

