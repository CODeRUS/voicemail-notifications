#include <sailfishapp.h>
#include <QGuiApplication>
#include <QQuickView>
#include <QtQml>

#include "systemdhelper.h"

int main(int argc, char *argv[])
{
    qmlRegisterType<SystemdHelper>("org.coderus.voicemailnotifications", 1, 0, "SystemdHelper");

    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    QScopedPointer<QQuickView> view(SailfishApp::createView());
    view->setSource(SailfishApp::pathTo("qml/voicemail-notifications.qml"));
    view->showFullScreen();

    return app->exec();
}


