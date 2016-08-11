import QtQuick 2.5
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.4

Rectangle {
    anchors.fill: parent
    width: 800
    height: 600

    SplitView {
        id: splitView
        anchors.fill: parent
        x: 0
        y: 0
        width: 800
        height: 600
    }
}
