import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2
import QtCharts 2.2
import Qt.Widgets 1.0
import TimelineTracker 1.0

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
    property alias flashHelpIndicator: helpButton.flashBackground

    signal addTimeClicked(date start, date end)
    signal removeTimeEntry(int index)
    signal closeRequested

    onClosing: {
        close.accepted = false;
        closeRequested();
    }

    Component.onCompleted: {
        if(minimumWidth >= 1280) {
            console.log("Exceeded recommended minimum width: " + minimumWidth);
        }
        if(minimumHeight >= 680) {
            console.log("Exceeded recommended minimum height: " + minimumHeight);
        }
    }

    SystemPalette {
        id: syspalActive
        colorGroup: SystemPalette.Active
    }

    Rectangle {
        id: header
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        gradient: Gradient {
            GradientStop {
                position: 0.0
                color: helpButton.checked ? syspalActive.alternateBase : "transparent"
            }
            GradientStop {
                position: (contactLabel.y + contactLabel.paintedHeight) / header.height
                color: helpButton.checked ? syspalActive.alternateBase : "transparent"
            }
            GradientStop {
                position: 1.0
                color: "transparent"
            }
        }

        z: mainLayout.z + 1
        implicitHeight: headerLayout.implicitHeight + headerLayout.anchors.margins * 2

        RowLayout {
            id: headerLayout
            anchors {
                fill: parent
                margins: 5
            }

            Label {
                id: contactLabel
                readonly property string email: "alexey.m@scopicsoftware.com"
                text: qsTr("A simple time tracker.<br/>Contact: ") + "<a href=\"mailto:" + email + "\">" + email + "</a>"
                visible: helpButton.checked
                Layout.alignment: Qt.AlignTop | Qt.AlignLeft
                onLinkActivated: {
                    Qt.openUrlExternally(link);
                }

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.RightButton
                    onClicked: {
                        contactContextMenu.x = mouse.x;
                        contactContextMenu.y = mouse.y;
                        contactContextMenu.open();
                    }
                }

                Menu {
                    id: contactContextMenu
                    MenuItem {
                        text: qsTr("Copy link")
                        onTriggered: {
                            Util.copyToClipboard(contactLabel.email);
                        }
                    }
                }
            }

            Item { Layout.fillWidth: true }

            RoundButton {
                id: helpButton
                property bool flashBackground: true
                background: Rectangle {
                    implicitHeight: Util.pixelMetric(QStyle.PM_LargeIconSize)
                    implicitWidth: implicitHeight
                    radius: helpButton.radius
                    color: helpButton.checked ? syspalActive.highlight : syspalActive.button
                    SequentialAnimation on color {
                        loops: Animation.Infinite
                        running: helpButton.flashBackground

                        ColorAnimation {
                            from: syspalActive.button
                            to: syspalActive.highlight
                            duration: 1000
                        }
                        ColorAnimation {
                            from: syspalActive.highlight
                            to: syspalActive.button
                            duration: 1000
                        }
                    }
                }
                contentItem: Image {
                    source: "image://pixmapProvider/stdpixmap/" + QStyle.SP_TitleBarContextHelpButton
                }
                checkable: true

                onCheckedChanged: {
                    flashBackground = false;
                }
            }
        }
    }

    ColumnLayout {
        id: mainLayout
        anchors {
            fill: parent
            margins: 5
        }

        Item {
            implicitHeight: chartViewGroupbox.implicitHeight
            implicitWidth: chartViewGroupbox.implicitWidth
            Layout.minimumHeight: 100
            Layout.minimumWidth: 100
            Layout.fillWidth: true
            Layout.fillHeight: true

            GroupBox {
                id: chartViewGroupbox
                title: qsTr("Timeline")
                padding: 0
                anchors.fill: parent

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

            HelpItem {
                visible: helpButton.checked
                text: qsTr("Chart representing time intervals")
            }
        }

        RowLayout {
            spacing: 0
            readonly property int spacingPrivate: 6
            Layout.fillWidth: true
            Layout.maximumHeight: dateTimeLayout.implicitHeight

            Item { Layout.fillWidth: true }

            Item {
                implicitHeight: timelineGroupbox.implicitHeight
                implicitWidth: timelineGroupbox.implicitWidth
                Layout.fillHeight: true
                Layout.preferredWidth: intervalsFontMetrics.boundingRect(timeModelStdText).width +
                                       timelineGroupbox.padding * 2 +
                                       intervalsFontMetrics.averageCharacterWidth

                GroupBox {
                    id: timelineGroupbox
                    title: qsTr("Intervals")
                    padding: 1
                    anchors.fill: parent

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

                HelpItem {
                    visible: helpButton.checked
                    title: qsTr("4")
                    text: qsTr("List with current time intervals")
                }
            }

            Item { Layout.preferredWidth: parent.spacingPrivate }

            Item {
                Layout.fillHeight: true
                Layout.maximumWidth: {
                    var w = 0;
                    for(var i = 0; i < buttonsLayout.children.length; ++i) {
                        w = Math.max(w, buttonsLayout.children[i].implicitWidth);
                    }
                    return w;
                }
                implicitHeight: buttonsLayout.implicitHeight
                implicitWidth: buttonsLayout.implicitWidth

                ColumnLayout {
                    id: buttonsLayout
                    anchors.fill: parent

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

                HelpItem {
                    visible: helpButton.checked
                    title: qsTr("3")
                    text: qsTr("Add or remove time")
                }
            }

            Item { Layout.preferredWidth: parent.spacingPrivate }

            RowLayout {
                id: dateTimeLayout
                Layout.fillWidth: true
                Layout.fillHeight: true

                Item {
                    implicitHeight: startDateGroupbox.implicitHeight
                    implicitWidth: startDateGroupbox.implicitWidth

                    GroupBox {
                        id: startDateGroupbox
                        title: qsTr("Start")

                        DateTimeWidget {
                            id: startDate
                        }
                    }

                    HelpItem {
                        visible: helpButton.checked
                        title: qsTr("1")
                        text: qsTr("Enter interval start")
                    }
                }

                Item {
                    implicitHeight: endDateGroupbox.implicitHeight
                    implicitWidth: endDateGroupbox.implicitWidth

                    GroupBox {
                        id: endDateGroupbox
                        title: qsTr("End")

                        DateTimeWidget {
                            id: endDate
                        }
                    }

                    HelpItem {
                        visible: helpButton.checked
                        title: qsTr("2")
                        text: qsTr("Enter interval end")
                    }
                }
            }

            Item { Layout.preferredWidth: parent.spacingPrivate }

            Item {
                implicitHeight: totalTimeGroupbox.implicitHeight
                implicitWidth: totalTimeGroupbox.implicitWidth
                Layout.fillHeight: true

                GroupBox {
                    id: totalTimeGroupbox
                    title: qsTr("Total Time")
                    anchors.fill: parent

                    Label {
                        id: totalTimeLabel
                        anchors.centerIn: parent
                        horizontalAlignment: Text.AlignHCenter
                    }
                }

                HelpItem {
                    visible: helpButton.checked
                    title: qsTr("5")
                    text: qsTr("Total time shown here")
                }
            }

            Item { Layout.fillWidth: true }
        }
    }

    MouseArea {
        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        enabled: helpButton.checked

        onClicked: {
            helpButton.checked = false
        }
    }
}
