import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

ApplicationWindow {
    id: mainWindow
    visible: true
    width: toolsLayout.implicitWidth * 2
    height: 480
    minimumHeight: mainLayout.implicitHeight + mainLayout.anchors.margins * 2
    minimumWidth: mainLayout.implicitWidth + mainLayout.anchors.margins * 2
    title: qsTr("Timeline Tracker")

    property var timeModel
    property alias totalTimeText: totalTimeLabel.text

    signal addTimeClicked(date start, date end)
    signal removeTimeEntry(int index)

    SystemPalette {
        id: syspalActive
        colorGroup: SystemPalette.Active
    }

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
                id: timelineListView
                model: timeModel
                anchors.fill: parent
                clip: true

                delegate: Rectangle {
                    implicitHeight: totalSpanLabel.paintedHeight + 10
                    implicitWidth: totalSpanLabel.paintedWidth + 10
                    color: ListView.isCurrentItem ?
                               syspalActive.highlight :
                               (index % 2 ?
                                    syspalActive.alternateBase :
                                    syspalActive.base)

                    Label {
                        id: totalSpanLabel
                        text: totalSpan
                        anchors.centerIn: parent
                        color: ListView.isCurrentItem ? syspalActive.highlightedText : syspalActive.text
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            timelineListView.currentIndex = index;
                        }
                    }
                }

                ScrollBar.vertical: ScrollBar {
                    parent: timelineListView.parent
                    anchors {
                        top: timelineListView.top
                        left: timelineListView.right
                        bottom: timelineListView.bottom
                    }
                }

                DelayButton {
                    id: removeRowButton
                    visible: timelineListView.currentIndex >= 0
                    x: timelineListView.currentItem !== null ?
                           Math.min(timelineListView.currentItem.x + timelineListView.currentItem.width, timelineListView.width - width) :
                           0
                    y: timelineListView.currentItem !== null ?
                           timelineListView.currentItem.y :
                           0
                    height: timelineListView.currentItem !== null ? timelineListView.currentItem.height : 0
                    width: height
                    text: "x"
                    delay: 1000
                    transition: Transition {
                        NumberAnimation {
                            duration: removeRowButton.delay
                            properties: "progress"
                            easing.type: Easing.OutCirc
                        }
                    }

                    onActivated: {
                        removeTimeEntry(timelineListView.currentIndex);
                        toggle();
                    }
                }
            }
        }

        ColumnLayout {
            id: toolsLayout
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
                    addTimeClicked(new Date(startDate.date.getFullYear(),
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
