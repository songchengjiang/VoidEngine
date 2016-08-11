import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Controls 1.4

Menu {
    title: qsTr("Elements")
    MenuItem {
        text: qsTr("Node")
    }
    MenuItem {
        text: qsTr("Camera")
    }
    MenuSeparator { }
    Menu {
        title: qsTr("Renderable Object")
        MenuItem {
            text: qsTr("Cone")
        }
        MenuItem {
            text: qsTr("Sphere")
        }
        MenuItem {
            text: qsTr("Entity")
        }
        MenuItem {
            text: qsTr("Particle System")
        }
    }
    MenuSeparator { }
    Menu {
        title: qsTr("Light")
        MenuItem {
            text: qsTr("Directional Light")
        }
        MenuItem {
            text: qsTr("Point Light")
        }
        MenuItem {
            text: qsTr("Spot Light")
        }
    }
    MenuSeparator { }
    Menu {
        title: qsTr("UI")
        MenuItem {
            text: qsTr("Text")
        }
        MenuItem {
            text: qsTr("Image")
        }
    }
    MenuSeparator { }
    MenuItem {
        text: qsTr("Skybox")
    }
}
