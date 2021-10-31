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
		RowLayout {
			spacing: 0
			Label {
				text: severity ? severity : "info"
				Layout.preferredWidth: backend.theme.entrySeverityWidth
				background: Rectangle {
					color: severityColor
					opacity: 0.5
				}
			}
			Label {
				text: logIdentity ? logIdentity : sourceId ? sourceId : "-"
				Layout.fillWidth: true
				background: Rectangle {
					color: severityColor
					opacity: 0.5
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
			Label {
				text: Format.remainingTimeStr(remainingTime)
				Layout.preferredWidth: backend.theme.entryReltimeWidth
			}
		}
	}
}
