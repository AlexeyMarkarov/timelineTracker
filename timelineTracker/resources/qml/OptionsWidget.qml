import QtQuick 2.7
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import Qt.Widgets 1.0
import TimelineTracker 1.0

Rectangle {
    implicitHeight: mainLayout.implicitHeight + mainLayout.margins * 2
    implicitWidth: mainLayout.implicitWidth + mainLayout.margins * 2
    color: syspalActive.window
    border {
        width: 1
        color: syspalActive.shadow
    }

    SystemPalette {
        id: syspalActive
        colorGroup: SystemPalette.Active
    }

    ListModel {
        id: categoriesModel

        // EXAMPLE USAGE
        /*ListElement {
            categoryName: qsTr("Dummy Element Red")
            categoryIconPath: "image://pixmapProvider/stdpixmap/SP_MessageBoxCritical"
        }*/

    }

    RowLayout {
        id: mainLayout
        anchors.fill: parent

        ListView {
            id: categoriesListView
            model: categoriesModel
            orientation: Qt.Vertical
            Layout.fillHeight: true
            Layout.minimumWidth: Util.pixelMetric(QStyle.PM_IconViewIconSize) * 3

            delegate: Rectangle {
                color: ListView.isCurrentItem ? syspalActive.highlight : "transparent"
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

        StackLayout {
            currentIndex: categoriesListView.currentIndex
            Layout.fillHeight: true
            Layout.fillWidth: true

            // EXAMPLE USAGE
            /*Rectangle {
                color: "red"
                implicitHeight: 100
                implicitWidth: 100
            }*/

        }
    }
}
