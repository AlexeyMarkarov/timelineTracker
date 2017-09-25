import QtQuick 2.7
import TimelineTracker 1.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import Qt.Widgets 1.0

Rectangle {
    property alias title: titleLabel.text
    property alias text: textLabel.text

    anchors.fill: parent
    radius: Util.pixelMetric(QStyle.PM_SmallIconSize)
    border {
        color: syspalActive.highlight
        width: Util.pixelMetric(QStyle.PM_DefaultFrameWidth)
    }
    color: Qt.rgba(syspalActive.highlight.r, syspalActive.highlight.g, syspalActive.highlight.b, 0.5)

    SystemPalette {
        id: syspalActive
        colorGroup: SystemPalette.Active
    }

    ColumnLayout {
        anchors.fill: parent

        Label {
            id: titleLabel
            color: syspalActive.highlightedText
            horizontalAlignment: Label.AlignHCenter
            verticalAlignment: Label.AlignVCenter
            font.pointSize: 20
            Layout.fillWidth: true
        }

        Label {
            id: textLabel
            color: syspalActive.highlightedText
            horizontalAlignment: Label.AlignHCenter
            verticalAlignment: Label.AlignVCenter
            font.pointSize: 16
            wrapMode: Text.WordWrap
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
}
