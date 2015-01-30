import QtQuick 2.1
import Sailfish.Silica 1.0
import org.coderus.voicemailnotifications 1.0
import "pages"

ApplicationWindow
{
    initialPage: Component { FirstPage { } }
    cover: Qt.resolvedUrl("cover/CoverPage.qml")

    property alias helper: helper

    SystemdHelper {
        id: helper
    }
}


