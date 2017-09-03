import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2
import QtCharts 2.2
import Qt.Widgets 1.0

ApplicationWindow {
    id: mainWindow
    visible: true
    minimumHeight: Math.max(mainLayout.implicitHeight + mainLayout.anchors.margins * 2, 600)
    minimumWidth: mainLayout.implicitWidth + mainLayout.anchors.margins * 2
    title: qsTr("Timeline Tracker")

    property var timeModel
    property string timeModelStdText: ""
    property alias totalTimeText: totalTimeLabel.text
    property alias chart: chartView

    signal addTimeClicked(date start, date end)
    signal removeTimeEntry(int index)
    signal closeRequested

    onClosing: {
        close.accepted = false;
        closeRequested();
    }

    Component.onCompleted: {
        if(minimumWidth >= 1280) {
            console.log("Exceeded recommended minimum width: " + minimumWidth)
        }
        if(minimumHeight >= 680) {
            console.log("Exceeded recommended minimum height: " + minimumHeight)
        }
    }

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

        RowLayout {
            Layout.fillWidth: true

            Item { Layout.fillWidth: true }

            RoundButton {
                background: Rectangle {
                    implicitHeight: parent.contentItem.implicitHeight
                    implicitWidth: parent.contentItem.implicitWidth
                    radius: parent.radius
                    color: syspalActive.button
                }
                contentItem: Image {
                    source: "image://pixmapProvider/stdpixmap/" + QStyle.SP_TitleBarContextHelpButton
                }
            }
        }

        GroupBox {
            title: qsTr("Timeline")
            padding: 0
            Layout.minimumHeight: 100
            Layout.minimumWidth: 100
            Layout.fillWidth: true
            Layout.fillHeight: true

            ChartView {
                id: chartView
                anchors.fill: parent
                margins {
                    top: 0
                    bottom: 0
                    left: 0
                    right: 0
                }
            }
        }

        RowLayout {
            spacing: 0
            readonly property int spacingPrivate: 6
            Layout.fillWidth: true
            Layout.maximumHeight: dateTimeLayout.implicitHeight

            Item { Layout.fillWidth: true }

            GroupBox {
                title: qsTr("Intervals")
                padding: 1
                Layout.fillHeight: true
                Layout.preferredWidth: intervalsFontMetrics.boundingRect(timeModelStdText).width +
                                       padding * 2 +
                                       intervalsFontMetrics.averageCharacterWidth

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
                        implicitWidth: totalSpanLabel.paintedWidth + intervalsFontMetrics.averageCharacterWidth
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
                            horizontalAlignment: Text.AlignHCenter
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                timelineListView.currentIndex = index;
                            }
                        }
                    }

                    ScrollBar.vertical: ScrollBar {}
                }
            }

            Item { Layout.preferredWidth: parent.spacingPrivate }

            ColumnLayout {
                Layout.fillHeight: true
                Layout.maximumWidth: {
                    var w = 0;
                    for(var i = 0; i < children.length; ++i) {
                        w = Math.max(w, children[i].implicitWidth);
                    }
                    return w;
                }

                Item { Layout.fillHeight: true }

                Button {
                    text: qsTr("<-- Add <--")
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

                DelayButton {
                    id: removeButton
                    text: qsTr("--> Remove <--")
                    enabled: timelineListView.currentIndex >= 0
                    Layout.fillWidth: true

                    delay: 1000
                    transition: Transition {
                        NumberAnimation {
                            duration: removeButton.delay
                            properties: "progress"
                            easing.type: Easing.OutCirc
                        }
                    }

                    onActivated: {
                        removeTimeEntry(timelineListView.currentIndex);
                    }
                    onReleased: {
                        checked = false;
                    }
                }

                Item { Layout.fillHeight: true }
            }

            Item { Layout.preferredWidth: parent.spacingPrivate }

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

            Item { Layout.preferredWidth: parent.spacingPrivate }

            GroupBox {
                title: qsTr("Total Time")
                Layout.fillHeight: true

                Label {
                    id: totalTimeLabel
                    anchors.centerIn: parent
                    horizontalAlignment: Text.AlignHCenter
                }
            }

            Item { Layout.fillWidth: true }
        }
    }
}
