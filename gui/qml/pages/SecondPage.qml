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
        }

        VerticalScrollDecorator {}
    }
}
