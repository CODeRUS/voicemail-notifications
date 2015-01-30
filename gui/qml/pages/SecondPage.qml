import QtQuick 2.1
import Sailfish.Silica 1.0

Page {
    id: page
    SilicaFlickable {
        id: flick
        anchors.fill: page
        contentHeight: content.height

        Column {
            id: content
            width: parent.width
            spacing: Theme.paddingLarge

            PageHeader {
                title: "About"
            }

            Label {
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: Theme.paddingLarge
                }
                textFormat: Text.RichText
                wrapMode: Text.Wrap
                text: "<style type=\"text/css\">a:link {color:" + Theme.highlightColor + ";}</style>Voicemail checker<br />by coderus in 0x7DF<br /><br />Source code: <a href=\"https://github.com/CODeRUS/voicemail-notifications\">Github</a>"
                onLinkActivated: Qt.openUrlExternally(link)
            }
        }

        VerticalScrollDecorator {}
    }
}
