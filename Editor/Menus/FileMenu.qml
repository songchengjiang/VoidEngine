import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.0

Menu {
    title: qsTr("File")
    MenuItem {
        text: qsTr("&New Project...")
    }
    MenuItem {
        text: qsTr("&Open Project...")
    }
    MenuItem {
        text: qsTr("&Save Project...")
    }
    MenuSeparator { }
    MenuItem {
        text: qsTr("New Scene")
    }
    MenuItem {
        text: qsTr("&Open Scene...")
    }
    MenuSeparator { }
    MenuItem {
        text: qsTr("Save Scene")
    }
    MenuItem {
        text: qsTr("&Save Scene As...")
    }
    MenuSeparator { }
    MenuItem {
        text: qsTr("Build")
    }
}
