///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///
import QtQuick 2.3
import QtQuick.Controls 2.4
import QtQuick.Dialogs 1.3
import QtQuick.Controls.Material 2.4
import QtQuick.Layouts 1.5
import "qrc:///views"

ApplicationWindow {
    id: root
    visible: true
    width: 1200
    height: 800
    Material.theme: backend.theme.light ? Material.Light : Material.Dark
    Material.accent: Material.Blue

    Action {
        id: quitAction
        text: qsTr("&Quit")
        shortcut: StandardKey.Quit
        onTriggered: {
            Qt.callLater(Qt.quit)
        }
    }

    Action {
        id: lightThemeToggleAction
        text: qsTr("&Light")
        checkable: true
        checked: backend.theme.light
        onToggled: {
            backend.theme.light = checked
        }
    }

    menuBar: MenuBar {
        Menu {
            title: qsTr("&File")
            MenuItem {
                action: quitAction
            }
        }
        Menu {
            title: qsTr("&Window")
            Menu {
                title: qsTr("&Theme")
                MenuItem {
                    action: lightThemeToggleAction
                }
            }
        }
    }

    contentData: ColumnLayout {
        anchors.fill: parent

        TabBar {
            id: mainTabBar
            Layout.fillWidth: true
            TabButton {
                text: qsTr("Entries")
            }
            TabButton {
                text: qsTr("Charts")
            }
            TabButton {
                text: qsTr("Progress")
            }
        }

        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            currentIndex: mainTabBar.currentIndex

            EntriesView {
				id: entriesView
                Layout.fillWidth: true
                Layout.fillHeight: true

                model: backend.entryLog.entries
            }

            ChartsView {
				id: chartsView
                Layout.fillWidth: true
                Layout.fillHeight: true

                model: backend.entryLog.charts
            }

            ProgressView {
				id: progressView
                Layout.fillWidth: true
                Layout.fillHeight: true

                model: backend.entryLog.progress
            }
        }
    }
}
