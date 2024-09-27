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

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (addPointEnabled) {
                        // Получаем координаты клика
                        var coordinate = map.toCoordinate(Qt.point(mouse.x, mouse.y));
                        var latitude = coordinate.latitude;
                        var longitude = coordinate.longitude;

                        // Создаем объект точки
                        var point = { latitude: latitude, longitude: longitude };

                        // Вызываем функцию для добавления точки на карту
                        addMapPoint(point);

                        // Вызываем сигнал для передачи координат в C++
                        pointClicked(latitude, longitude);
                    } else {
                        console.log("Adding points is disabled.");
                    }
                }
            }


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

    // Переменная для контроля возможности добавления точек
    property bool addPointEnabled: false

    signal pointClicked(double latitude, double longitude)

    function setAddPointMode(enable) {
            addPointEnabled = enable;
            console.log("Add point mode:", addPointEnabled);
        }

    function addMapPoint(point) {
        console.log("Adding point to map:", point.latitude, point.longitude);
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

    function clearMapItems() {
        pointsModel.clear();  // Очищаем модель точек
        polyline.path = [];   // Очищаем линию
        console.log("All points and lines cleared.");
    }

    signal pointsRetrieved(var points)

    function getAllPoints() {
        var allPoints = [];
        for (var i = 0; i < pointsModel.count; i++) {
            var point = {
                latitude: pointsModel.get(i).latitude,
                longitude: pointsModel.get(i).longitude
            };
            allPoints.push(point);
        }
        console.log("All points:", allPoints); // Вывод всех точек в консоль
        pointsRetrieved(allPoints); // Сигнал для передачи точек обратно
    }


}
