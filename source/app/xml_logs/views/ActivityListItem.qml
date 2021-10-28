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
	RowLayout {
		Label {
			text: sourceId ? sourceId : "-"
			Layout.preferredWidth: backend.theme.entrySourceWidth
		}
	}
}
