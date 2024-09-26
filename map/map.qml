import QtQuick 2.12
import QtQuick.Controls 2.12
import QtLocation 5.12
import QtPositioning 5.12

Rectangle {
    anchors.fill: parent

    Plugin {
        name: "osm"
        id: mapPlugin

        PluginParameter {
            name: "osm.mapping.custom.host"
            value: "http://tile.thunderforest.com/landscape/%z/%x/%y.png?apikey=73984ea8870e4de395ed43ef621aa14f&fake=.png"
        }
    }

    Map {
        id: map
        anchors.fill: parent
        plugin: mapPlugin
        center: QtPositioning.coordinate(55.7558, 37.6173)
        zoomLevel: 10
        activeMapType: supportedMapTypes[supportedMapTypes.length - 1]

        // Массив для хранения точек и линий
        MapItemView {
            id: pointsView
            model: ListModel { id: pointsModel }
            delegate: MapQuickItem {
                coordinate: QtPositioning.coordinate(model.latitude, model.longitude)
                sourceItem: Rectangle {
                    width: 10
                    height: 10
                    color: "red"
                    radius: 5
                }
            }
        }

        MapPolyline {
            id: polyline
            line.width: 3
            line.color: "blue"
            path: []  // Инициализация пустого пути
        }

    }

    function addMapPoint(point) {
        pointsModel.append({ latitude: point.latitude, longitude: point.longitude });
    }

    function addMapLine(path) {
        console.log("Path length:", path.length);

        // Создаем новый массив для координат
        var newPath = [];
        for (var i = 0; i < path.length; ++i) {
            var coord = QtPositioning.coordinate(path[i].latitude, path[i].longitude);
            console.log("Adding coordinate:", coord.latitude, coord.longitude);  // Отладочный вывод
            newPath.push(coord);  // Добавляем координаты в новый массив
        }

        polyline.path = newPath;  // Присваиваем новый массив
        console.log("Polyline path:", polyline.path);  // Вывод текущего пути
    }



}
