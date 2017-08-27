import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

Rectangle {
    color: "transparent"
    implicitWidth: mainLayout.implicitWidth
    implicitHeight: mainLayout.implicitHeight

    property alias date: calendar.selectedDate
    property alias hours: hoursControl.value
    property alias minutes: minutesControl.value

    ColumnLayout {
        id: mainLayout
        anchors.fill: parent

        RowLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true

            Label {
                text: qsTr("H:")
            }

            SpinBox {
                id: hoursControl
                maximumValue: 23
                Layout.fillWidth: true
            }

            Label {
                text: qsTr("M:")
            }

            SpinBox {
                id: minutesControl
                maximumValue: 59
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
