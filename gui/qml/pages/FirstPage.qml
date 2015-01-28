import QtQuick 2.1
import Sailfish.Silica 1.0
import org.coderus.voicemailnotifications 1.0

Page {
    id: page
    SilicaFlickable {
        id: flick
        anchors {
            fill: page
            leftMargin: Theme.paddingLarge
            rightMargin: Theme.paddingLarge
        }
        contentHeight: content.height

        PullDownMenu {
            MenuItem {
                text: "About"
                onClicked: pageStack.push(Qt.resolvedUrl("SecondPage.qml"))
            }
        }

        Column {
            id: content
            width: parent.width
            spacing: Theme.paddingLarge

            PageHeader {
                title: "Voicemail settings"
            }

            Label {
                text: "Service autostart: " + (helper.serviceEnabled ? "enabled" : "disabled")
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: helper.serviceEnabled ? "Disable" : "Enable"
                onClicked: helper.setEnabled(!helper.serviceEnabled)
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Force start"
                onClicked: helper.forceStart()
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Force exit"
                onClicked: helper.forceExit()
            }
        }

        VerticalScrollDecorator {}
    }

    SystemdHelper {
        id: helper
    }
}
