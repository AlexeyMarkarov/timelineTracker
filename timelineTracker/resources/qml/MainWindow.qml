import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtCharts 2.2

ApplicationWindow {
    id: mainWindow
    visible: true
    minimumHeight: mainLayout.implicitHeight + mainLayout.anchors.margins * 2
    minimumWidth: mainLayout.implicitWidth + mainLayout.anchors.margins * 2
    width: minimumWidth
    height: minimumHeight
    title: qsTr("Timeline Tracker")

    property var timeModel
    property alias totalTimeText: totalTimeLabel.text
    property alias chart: chartView

    signal addTimeClicked(date start, date end)
    signal removeTimeEntry(int index)

    SystemPalette {
        id: syspalActive
        colorGroup: SystemPalette.Active
    }

    ColumnLayout {
        id: mainLayout
        anchors {
            fill: parent
            margins: 5
        }

        GroupBox {
            title: qsTr("Timeline")
            Layout.minimumHeight: 100
            Layout.minimumWidth: 100
            Layout.fillWidth: true
            Layout.fillHeight: true

            ChartView {
                id: chartView
                anchors.fill: parent
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.maximumHeight: dateTimeLayout.implicitHeight

            Item { Layout.fillWidth: true }

            GroupBox {
                title: qsTr("Intervals")
                padding: 1
                Layout.fillHeight: true
                Layout.preferredWidth: (timeModel !== undefined ?
                                            intervalsFontMetrics.boundingRect(timeModel.getStdText()).width :
                                            100) +
                                       padding * 2 +
                                       intervalsFontMetrics.maximumCharacterWidth

                FontMetrics {
                    id: intervalsFontMetrics
                }

                ListView {
                    id: timelineListView
                    model: timeModel
                    anchors.fill: parent
                    clip: true

                    delegate: Rectangle {
                        implicitHeight: totalSpanLabel.paintedHeight + intervalsFontMetrics.height
                        implicitWidth: totalSpanLabel.paintedWidth + intervalsFontMetrics.maximumCharacterWidth
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
                            font: intervalsFontMetrics.font
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                timelineListView.currentIndex = index;
                            }
                        }
                    }

                    ScrollBar.vertical: ScrollBar {}

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

            Button {
                id: btnAdd
                text: qsTr("<-- Add")

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

            RowLayout {
                id: dateTimeLayout
                Layout.fillWidth: true
                Layout.fillHeight: true

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

            GroupBox {
                title: qsTr("Total Time")
                Layout.fillHeight: true

                Label {
                    id: totalTimeLabel
                    anchors.centerIn: parent
                }
            }

            Item { Layout.fillWidth: true }
        }
    }
}
