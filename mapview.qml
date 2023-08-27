import QtQuick 2.15
import QtPositioning
import QtLocation


Item {
    id: window
        Plugin{
            id: mapPlugin
            name:"osm"
        }
        Map{
            id: map
            anchors.fill: parent
            plugin: mapPlugin
            center: QtPositioning.coordinate(48.06, 20.47);
            zoomLevel: 14


            Timer
                {
                    id: mapRefreshtimer
                    running: true
                    interval: 5000
                    repeat: true
                    onTriggered:
                    {
                        map.fitViewportToMapItems()
                    }
                }

            MapItemView{
            model:marker_model
            delegate: MapQuickItem {

                coordinate: QtPositioning.coordinate(position.x, position.y)
                anchorPoint.x: image.width * 0.5
                anchorPoint.y: image.height

                sourceItem: Column {
                    Image { id: image; source: "marker.png" }
                    Text { text: model.label; font.bold: true }
                }
                MouseArea {
                                    anchors.fill: parent
                                    onClicked: (mouse)=> {
                                        mouse.accepted = false
                                        marker_model.removeMarker(index)
                                    }
                                }

              }
            }

            WheelHandler {
                id: wheel
                // workaround for QTBUG-87646 / QTBUG-112394 / QTBUG-112432:
                // Magic Mouse pretends to be a trackpad but doesn't work with PinchHandler
                // and we don't yet distinguish mice and trackpads on Wayland either
                acceptedDevices: Qt.platform.pluginName === "cocoa" || Qt.platform.pluginName === "wayland"
                                 ? PointerDevice.Mouse | PointerDevice.TouchPad
                                 : PointerDevice.Mouse
                rotationScale: 1/120
                property: "zoomLevel"
            }
            DragHandler {
                id: drag
                target: null
                onTranslationChanged: (delta) => map.pan(-delta.x, -delta.y)
            }
            Shortcut {
                enabled: map.zoomLevel < map.maximumZoomLevel
                sequence: StandardKey.ZoomIn
                onActivated: map.zoomLevel = Math.round(map.zoomLevel + 1)
            }
            Shortcut {
                enabled: map.zoomLevel > map.minimumZoomLevel
                sequence: StandardKey.ZoomOut
                onActivated: map.zoomLevel = Math.round(map.zoomLevel - 1)
            }
        }
}
