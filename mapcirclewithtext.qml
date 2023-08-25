import QtQuick 2.15
import QtLocation 5.15
import QtQuick.Controls 2.15

MapCircleItem{
    property coordinate center
    property real radius
    property string text

    MapCircle {
        center: center
        radius: radius
        color: "blue"
        border.color: "black"
    }

    MapQuickItem {
        anchorPoint.x: marker.width / 2
        anchorPoint.y: marker.height

        coordinate: center
        sourceItem: Text {
            id: marker
            text: parent.text
            font.pixelSize: 14
            color: "black"
            wrapMode: Text.WordWrap
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }
}
