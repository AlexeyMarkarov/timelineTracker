import QtQuick 2.7
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0
import Qt.Widgets 1.0
import TimelineTracker 1.0

Rectangle {
    implicitHeight: mainLayout.implicitHeight + mainLayout.margins * 2
    implicitWidth: mainLayout.implicitWidth + mainLayout.margins * 2
    color: syspalActive.window
    readonly property int borderWidth: 1
    border {
        width: borderWidth
        color: syspalActive.shadow
    }

    SystemPalette {
        id: syspalActive
        colorGroup: SystemPalette.Active
    }

    ListModel {
        id: categoriesModel

        ListElement {
            categoryName: qsTr("Analytics")
            categoryIconPath: "image://pixmapProvider/stdpixmap/SP_DriveNetIcon"
        }
    }

    RowLayout {
        id: mainLayout
        anchors {
            fill: parent
            margins: borderWidth
        }
        spacing: 0

        ListView {
            id: categoriesListView
            model: categoriesModel
            orientation: Qt.Vertical
            Layout.fillHeight: true
            Layout.minimumWidth: Util.pixelMetric(QStyle.PM_IconViewIconSize) * 3

            delegate: Rectangle {
                color: ListView.isCurrentItem ?
                           Qt.rgba(syspalActive.highlight.r, syspalActive.highlight.g, syspalActive.highlight.b, 200/255) :
                           "transparent"
                width: parent.width
                height: Util.pixelMetric(QStyle.PM_IconViewIconSize) * 2

                ColumnLayout {
                    id: listDelegateLayout
                    anchors.fill: parent
                    spacing: 0

                    Item { Layout.fillHeight: true }

                    Image {
                        source: categoryIconPath
                        Layout.alignment: Qt.AlignHCenter
                        Layout.minimumHeight: Util.pixelMetric(QStyle.PM_IconViewIconSize)
                        Layout.minimumWidth: Util.pixelMetric(QStyle.PM_IconViewIconSize)
                    }

                    Label {
                        text: categoryName
                        wrapMode: Label.Wrap
                        color: ListView.isCurrentItem ? syspalActive.highlightedText : syspalActive.text
                        horizontalAlignment: Label.AlignHCenter
                        Layout.fillWidth: true
                    }

                    Item { Layout.fillHeight: true }
                }

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.AllButtons
                    onClicked: {
                        categoriesListView.currentIndex = index;
                    }
                }
            }
        }

        Rectangle {
            Layout.preferredWidth: 1
            Layout.fillHeight: true
            color: syspalActive.shadow
        }

        Rectangle {
            color: syspalActive.window
            Layout.fillHeight: true
            Layout.fillWidth: true

            StackLayout {
                anchors.fill: parent
                currentIndex: categoriesListView.currentIndex

                Item {
                    ColumnLayout {
                        anchors.fill: parent

                        DetailedSwitch {
                            text: qsTr("Send anonymous analytics data")
                            detailedText: qsTr("Analytics help to improve the product by providing feedback on app usage.\n" +
                                               "Data includes anonymized IP address, screen resolution, OS version, system language, and UI interactions.\n" +
                                               "Powered by Google Analytics.");
                            Layout.fillWidth: true
                        }

                        Item { Layout.fillHeight: true }
                    }
                }
            }
        }
    }
}
