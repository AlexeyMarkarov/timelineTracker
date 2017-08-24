import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

Rectangle {
    color: "transparent"
    implicitWidth: mainLayout.implicitWidth
    implicitHeight: mainLayout.implicitHeight

    property alias date: calendar.selectedDate
    property alias hours: hoursTumbler.currentIndex
    property alias minutes: minutesTumbler.currentIndex

    Component {
        id: tumblerComponent

        Label {
            id: label
            text: modelData
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            opacity: 1.0 - Math.abs(Tumbler.displacement) / (Tumbler.tumbler.visibleItemCount / 2)
            font.pointSize: 20
        }
    }

    ColumnLayout {
        id: mainLayout
        anchors.fill: parent

        RowLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true

            Tumbler {
                id: hoursTumbler
                model: 24
                delegate: tumblerComponent
                Layout.fillWidth: true
            }
            Tumbler {
                id: minutesTumbler
                model: 60
                delegate: tumblerComponent
                Layout.fillWidth: true
            }
        }

        Calendar {
            id: calendar
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
}
