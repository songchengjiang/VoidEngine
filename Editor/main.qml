import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import "./Menus"

ApplicationWindow {
    
    //title of the application
    visible: true
    title: qsTr("VoidEngine")
    width: 800
    height: 600
    
    Action {
        id: undoAction
        text: qsTr("&Undo")
        shortcut: StandardKey.Undo
        iconName: "edit-undo"
        enabled: (!!activeFocusItem && !!activeFocusItem["undo"])
        onTriggered: activeFocusItem.undo()
    }
    
    Action {
        id: redoAction
        text: qsTr("&Redo")
        shortcut: StandardKey.Redo
        iconName: "edit-redo"
        enabled: (!!activeFocusItem && !!activeFocusItem["redo"])
        onTriggered: activeFocusItem.redo()
    }
    
    Action {
        id: copyAction
        text: qsTr("&Copy")
        shortcut: StandardKey.Copy
        iconName: "edit-copy"
        enabled: (!!activeFocusItem && !!activeFocusItem["copy"])
        onTriggered: activeFocusItem.copy()
    }
    
    Action {
        id: cutAction
        text: qsTr("Cu&t")
        shortcut: StandardKey.Cut
        iconName: "edit-cut"
        enabled: (!!activeFocusItem && !!activeFocusItem["cut"])
        onTriggered: activeFocusItem.cut()
    }
    
    Action {
        id: pasteAction
        text: qsTr("&Paste")
        shortcut: StandardKey.Paste
        iconName: "edit-paste"
        enabled: (!!activeFocusItem && !!activeFocusItem["paste"])
        onTriggered: activeFocusItem.paste()
    }
    
    //menu containing two menu items
    menuBar: MenuBar {
        FileMenu {}
        EditMenu {}
        ElementsMenu {}
    }
    
    //Content Area
    
    MainForm {
        anchors.fill: parent
    }
}
