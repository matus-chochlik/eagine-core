///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///
import QtQuick 2.2
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2

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
		entryListItem.view.currentIndex = index
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

	ColumnLayout {
		Item {
			Layout.preferredWidth: 10
		}
		Label {
			text: format ? format : "-"
		}
		Label {
			text: sourceId ? sourceId : "-"
		}
	}
}
