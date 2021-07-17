///
/// Copyright Matus Chochlik.
/// Distributed under the GNU GENERAL PUBLIC LICENSE version 3.
/// See http://www.gnu.org/licenses/gpl-3.0.txt
///
import QtQuick 2.2
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2

ListView {
	id: entryListView
	delegate: Loader {
		id: itemLoader

		function itemViewUrl() {
			return "qrc:///views/EntryListItem.qml"
		}

		source: itemViewUrl()

		onLoaded: {
			itemLoader.item.view = entryListView
		}
	}
	spacing: 2
	focus: true
	clip: true
}

