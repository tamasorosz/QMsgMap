import QtQuick 2.15
import QtQuick.Controls 2.15
import QtPositioning 5.15
import QtLocation 5.15

Item {
    id: root
    anchors.fill: parent

    Plugin {
        id: mapPlugin
        name: "osm"
    }

    Map {
        id: map
        anchors.fill: parent
        plugin: mapPlugin
        center: QtPositioning.coordinate(48.06, 20.47)
        zoomLevel: 14

        MapItemView {
            model: marker_model

            delegate: MapQuickItem {

                coordinate: QtPositioning.coordinate(position.x, position.y)

                anchorPoint.x: image.width * 0.5
                anchorPoint.y: image.height

                sourceItem: Column {
                    spacing: 2

                    Image {
                        id: image
                        source: "qrc:/marker.png" // vagy qrc:/images/marker.png
                    }

                    Label {
                        text: model.label
                        font.pixelSize: 14
                        font.bold: true
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: function(mouse) {
                        mouse.accepted = false
                        marker_model.removeMarker(index)
                    }
                }
            }
        }
    }
}