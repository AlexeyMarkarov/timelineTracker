import QtQuick 2.5
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.1

Switch {
    id: root
    property string detailedText

    SystemPalette {
        id: syspalActive
        colorGroup: SystemPalette.Active
    }

    indicator.anchors.top: contentItem.top

    contentItem: ColumnLayout {
        Label {
            text: root.text
            font: root.font
            color: syspalActive.text
            leftPadding: root.indicator.width + root.spacing
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
        Label {
            text: root.detailedText
            font: root.font
            color: syspalActive.mid
            wrapMode: Label.Wrap
            leftPadding: root.indicator.width + root.spacing
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
}
