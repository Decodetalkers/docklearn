import QtQuick 2.11
import QtQuick.Controls 2.4

import Dock.Global 1.0

ApplicationWindow {
    visible: true
    //maximumHeight: 200
    //minimumHeight: 200
    ListView {
        anchors.fill : parent
        model: ElementGroupModel.groups
        orientation: ListView.Horizontal
        delegate: ItemDelegate {
            width: 200
            height: 200
            Image {
                width: 200
                height: 200
                source: "data:image/png;base64," + modelData.icon
                MouseArea {
                    id: _mouseArea
                    hoverEnabled: true
                    anchors.fill: parent
                }
                ToolTip.text: modelData.id
                ToolTip.visible: _mouseArea.containsMouse;
            }
        }
    }
}
