import QtQuick 2.12
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2
import "qrc:///views"

Pane {
    id: progressView
    property variant model: null
    leftPadding: 0

    ColumnLayout {
        anchors.fill: parent

        ActivityListView {
            id: activityListView
            Layout.fillWidth: true
            Layout.fillHeight: true

            model: progressView.model.activityList
        }
    }
}

