///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///
import QtQuick 2.2
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2
import "qrc:///scripts/Format.js" as Format
import "qrc:///views"

Control {
    id: entryListItem
    property variant view: null

    width: view.width
    height: 50
    leftPadding: 0
    topPadding: 0
    rightPadding: 0
    bottomPadding: 0
    clip: true

    background: Rectangle {
        id: nodeListItemBg
        color: "gray"
        opacity: 0.05
    }

    function isCurrent() {
        return entryListItem.view.currentIndex == index
    }

    function makeCurrent() {
        // TODO
        // entryListItem.view.currentIndex = index
    }

    function highlightOpacity() {
        return (isCurrent() ? 0.5 : 0.0)
    }

    state: isCurrent() ? "Highlighted" : "Default"
    states: [
        State {
            name: "Highlighted"
            PropertyChanges {
                target: nodeListItemBg
                opacity: highlightOpacity()
            }
        },
        State {
            name: "Default"
            PropertyChanges {
                target: nodeListItemBg
                opacity: 0.05
            }
        }
    ]

    transitions: [
        Transition {
            from: "Default"
            to: "Highlighted"
            PropertyAnimation {
                target: nodeListItemBg
                properties: "opacity"
                duration: 100
            }
        },
        Transition {
            from: "Highlighted"
            to: "Default"
            PropertyAnimation {
                target: nodeListItemBg
                properties: "opacity"
                duration: 300
            }
        }
    ]

    focus: true
    Keys.onPressed: {
        if(event.key == Qt.Key_Return) {
            makeCurrent()
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: makeCurrent()
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0
        Repeater {
            Layout.preferredWidth: 15 * streamCount
            model: streamCount

            Image {
                width: 15
                height: 50

                function connectorVert() {
                    if(streamPosition < 0) {
                        return "b"
                    }
                    if(streamPosition > 0) {
                        return "e"
                    }
                    return "c"
                }

                function connectorHorz() {
                    if(index < streamIndex) {
                        return "l"
                    }
                    if(index > streamIndex) {
                        return "r"
                    }
                    return "c"
                }

                function connectorLast() {
                    if(index + 1 == streamCount) {
                        return "t"
                    }
                    return "f"
                }

                function connectorThme() {
                    return backend.theme.light ? "light" : "dark"
                }

                function connectorImageSource() {
                    return "qrc:/images/conn_%1_%2_%3_%4.png"
                        .arg(connectorVert())
                        .arg(connectorHorz())
                        .arg(connectorLast())
                        .arg(connectorThme())
                }
                source: connectorImageSource()
            }
        }
        ColumnLayout {
            spacing: 0
            Control {
                Layout.fillWidth: true
				Layout.preferredHeight: backend.theme.entryHeaderHeight
                background: Rectangle {
                    color: severityColor
                    opacity: 0.5
                }
                RowLayout {
					anchors.fill: parent
                    spacing: 1
                    Label {
                        text: severity ? severity : "info"
                        Layout.preferredWidth: backend.theme.entrySeverityWidth
                    }
                    Label {
                        text: Format.durationStr(reltimeSec)
                        Layout.preferredWidth: backend.theme.entryReltimeWidth
                    }
                    Label {
                        text: sourceId ? sourceId : "-"
                        Layout.preferredWidth: backend.theme.entrySourceWidth
                    }
                    Label {
                        text: logIdentity
                        Layout.fillWidth: true
                    }
                }
            }
            Label {
                text: message ? message : format ? format : "-"
            }
        }
    }
}
