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
    id: activityListItem
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
    ColumnLayout {
        anchors.fill: parent
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
                    text: Format.durationStr(elapsedTime)
                    Layout.preferredWidth: backend.theme.entryReltimeWidth
                }
                Label {
                    text: Format.remainingTimeStr(remainingTime)
                    Layout.preferredWidth: backend.theme.entryReltimeWidth
                }
                Label {
                    text: sourceId ? sourceId : "-"
                    Layout.preferredWidth: backend.theme.entrySourceWidth
                }
                Label {
                    text: logIdentity ? logIdentity : "-"
                    Layout.fillWidth: true
                }
            }
        }
        RowLayout {
            ProgressBar {
                Layout.fillWidth: true
                from: progressMin
                value: progressValue
                to: progressMax
            }
        }
        RowLayout {
            Label {
                text: message ? message : "-"
                Layout.fillWidth: true
            }
        }
    }
}
