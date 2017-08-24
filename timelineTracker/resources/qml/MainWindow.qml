import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

ApplicationWindow {
    id: mainWindow
    visible: true
    width: 640
    height: 480
    minimumHeight: mainLayout.implicitHeight
    minimumWidth: mainLayout.implicitWidth
    title: qsTr("Timeline Tracker")

    property var timeModel
    property alias totalTimeText: totalTimeLabel.text

    signal addTime(date start, date end)

    RowLayout {
        id: mainLayout
        anchors {
            fill: parent
            margins: 5
        }

        GroupBox {
            title: qsTr("Timeline")
            Layout.fillHeight: true
            Layout.fillWidth: true

            ListView {
                model: timeModel
                Layout.fillHeight: true
                Layout.fillWidth: true
            }
        }

        ColumnLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.maximumWidth: dateTimeLayout.implicitWidth

            RowLayout {
                id: dateTimeLayout
                Layout.fillWidth: true

                GroupBox {
                    title: qsTr("Start")

                    DateTimeWidget {
                        id: startDate
                    }
                }

                GroupBox {
                    title: qsTr("End")

                    DateTimeWidget {
                        id: endDate
                    }
                }
            }

            Button {
                id: btnAdd
                text: qsTr("Add")
                Layout.fillWidth: true

                onClicked: {
                    addTime(new Date(startDate.date.getFullYear(),
                                     startDate.date.getMonth(),
                                     startDate.date.getDate(),
                                     startDate.hours,
                                     startDate.minutes),
                            new Date(endDate.date.getFullYear(),
                                     endDate.date.getMonth(),
                                     endDate.date.getDate(),
                                     endDate.hours,
                                     endDate.minutes));
                }
            }

            GroupBox {
                title: qsTr("Total Time")
                Layout.fillWidth: true

                Label {
                    id: totalTimeLabel
                    anchors.centerIn: parent
                    text: "000\n00h 00m\n0.0h"
                }
            }

            Item { Layout.fillHeight: true }
        }
    }
}
