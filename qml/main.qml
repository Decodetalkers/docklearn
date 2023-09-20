import QtQuick 2.11
import QtQuick.Controls 2.4

import Dock.Global 1.0

ApplicationWindow {
    visible: true
    maximumHeight: 200
    minimumHeight: 200
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
                source: "data:image/svg+xml;utf8,<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 96 105\"> <g fill=\"#97C024\" stroke=\"#97C024\" stroke-linejoin=\"round\" stroke-linecap=\"round\"> <path d=\"M14,40v24M81,40v24M38,68v24M57,68v24M28,42v31h39v-31z\" stroke-width=\"12\"/> <path d=\"M32,5l5,10M64,5l-6,10 \" stroke-width=\"2\"/> </g> <path d=\"M22,35h51v10h-51zM22,33c0-31,51-31,51,0\" fill=\"#97C024\"/> <g fill=\"#FFF\"> <circle cx=\"36\" cy=\"22\" r=\"2\"/> <circle cx=\"59\" cy=\"22\" r=\"2\"/> </g> </svg>"
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
