import QtQuick 2.12
import QtQuick.Controls 2.12
import QtLocation 5.12
import QtPositioning 5.12

Rectangle {
    anchors.fill: parent  // Заполняем всё доступное пространство

    Plugin {
        name: "osm"
        id: mapPlugin

        PluginParameter {
            name: "osm.mapping.custom.host"

            // OSM plugin will auto-append if .png isn't suffix, and that screws up apikey which silently
            // fails authentication (only Wireshark revealed it)
            value: "http://tile.thunderforest.com/landscape/%z/%x/%y.png?apikey=73984ea8870e4de395ed43ef621aa14f&fake=.png"
        }
    }

    Map {
        id: map
        anchors.fill: parent  // Карта также заполняет всё пространство
        plugin: mapPlugin
        center: QtPositioning.coordinate(55.7558, 37.6173)  // Москва
        zoomLevel: 10
        activeMapType: supportedMapTypes[supportedMapTypes.length - 1]

    }
}
