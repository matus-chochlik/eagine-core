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
	id: streamListItem
	property variant view: null

	width: view.width
	height: 80
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
				color: "gray"
				opacity: 0.5
			}
			RowLayout {
				anchors.fill: parent
				spacing: 1
				Label {
					text: logIdentity ? logIdentity : "-"
					Layout.preferredWidth: backend.theme.entrySourceWidth
				}
			}
		}
		GridLayout {
			columns: 6
			Layout.fillWidth: true
			Layout.fillHeight: true

			Label {
				text: "Branch"
				font.bold: true
			}
			Label {
				text: gitBranch ? gitBranch : "-"
				Layout.fillWidth: true
			}

			Label {
				text: "EAGine revision"
				font.bold: true
			}
			Label {
				text: gitDescribe? gitDescribe : "-"
				Layout.fillWidth: true
			}

			Label {
				text: "EAGine version"
				font.bold: true
			}

			Label {
				text: gitVersion ? gitVersion : "-"
				Layout.fillWidth: true
			}

			Label {
				text: "Architecture"
				font.bold: true
			}
			Label {
				text: architecture ? architecture : "-"
				Layout.fillWidth: true
			}

			Label {
				text: "Compiler"
				font.bold: true
			}
			Label {
				text: compilerName ? compilerName : "-"
				Layout.fillWidth: true
			}

			Label {
				text: "Compiler version"
				font.bold: true
			}
			Label {
				text: Format.version(
					compilerVersionMajor,
					compilerVersionMinor,
					compilerVersionPatch)
				Layout.fillWidth: true
			}

			Label {
				text: "OS name"
				font.bold: true
			}
			Label {
				text: osName ? osName : "-"
				Layout.fillWidth: true
			}

			Label {
				text: "OS code name"
				font.bold: true
			}
			Label {
				text: osCodeName ? osCodeName : "-"
				Layout.fillWidth: true
			}
		}
	}
}
