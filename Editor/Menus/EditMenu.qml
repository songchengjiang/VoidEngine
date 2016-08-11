import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Controls 1.4

Menu {
    title: qsTr("Edit")
    MenuItem {
        action: undoAction
    }
    MenuItem {
        action: redoAction
    }
    MenuSeparator { }
    MenuItem {
        action: cutAction
    }
    MenuItem {
        action: copyAction
    }
    MenuItem {
        action: pasteAction
    }
}
