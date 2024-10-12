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

                        if (addPointEnabled == 1)
                        {
                            // Вызываем функцию для добавления точки на карту
                            addMapPoint(point);

                            // Вызываем сигнал для передачи координат в C++
                            pointClicked(latitude, longitude);
                        } else if (addPointEnabled == 2) {
                            console.log("Adding point to map:", point.latitude, point.longitude);

                            setMarker(point)
                            pointClicked(point.latitude, point.longitude);
                        } else if (addPointEnabled == 3) {
                            console.log("Adding point to map:", point.latitude, point.longitude);

                            setGotoPoint(point)
                            pointClicked(point.latitude, point.longitude);
                        }

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

        // Массив для хранения репера (специального маркера)
        MapItemView {
            id: markerView
            model: ListModel { id: markerModel }
            delegate: MapQuickItem {
                coordinate: QtPositioning.coordinate(model.latitude, model.longitude)
                sourceItem: Rectangle {
                    width: 12
                    height: 12
                    color: "green"  // Цвет репера
                    radius: 6
                }
            }
        }

        // Массив для хранения маркера агента
        MapItemView {
            id: agentView
            model: ListModel { id: agentModel }
            delegate: MapQuickItem {
                coordinate: QtPositioning.coordinate(model.latitude, model.longitude)
                sourceItem: Rectangle {
                    width: 12
                    height: 12
                    color: "lightpink"  // Цвет маркера агента
                    radius: 6
                }
            }
        }

        // Массив для хранения выхода в точку
        MapItemView {
            id: missionGoto
            model: ListModel { id: goto_point }
            delegate: MapQuickItem {
                coordinate: QtPositioning.coordinate(model.latitude, model.longitude)
                sourceItem: Rectangle {
                    width: 10
                    height: 10
                    color: "black"  // Цвет маркера агента
                    radius: 6
                }
            }
        }
    }

    // Переменная для контроля возможности добавления точек
    property int addPointEnabled: 0

    signal pointClicked(double latitude, double longitude)
    signal pointsRetrieved(var points)

    // Функция для включения/выключения режима добавления точек
    // 1 - точки для покрытия
    // 2 - точки для
    function setAddPointMode(enable) {
            addPointEnabled = enable;
            console.log("Add point mode:", addPointEnabled);
        }

    // Функция для добавления точки на карту
    function addMapPoint(point) {
        console.log("Adding point to map:", point.latitude, point.longitude);
        pointsModel.append({ latitude: point.latitude, longitude: point.longitude });
    }

    // Функция для добавления линии на карту
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

    // Функция для очистки всех точек и линий, кроме репера
    function clearMapItems() {
        pointsModel.clear();  // Очищаем модель точек
        polyline.path = [];   // Очищаем линию
        console.log("All points and lines cleared, except the marker.");
    }

    // Функция для получения всех точек
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

    // Функция для добавления репера
    function setMarker(point) {
        markerModel.clear();  // Очищаем старый репер
        markerModel.append({ latitude: point.latitude, longitude: point.longitude });
    }

    // Функция для удаления репера (если необходимо)
    function removeMarker() {
        markerModel.clear();  // Очищаем репер
    }

    // Функция для добавления репера
    function setGotoPoint(point) {
        goto_point.clear();  // Очищаем старый репер
        goto_point.append({ latitude: point.latitude, longitude: point.longitude });
    }

    // Функция для удаления репера (если необходимо)
    function removeGotoPoint() {
        goto_point.clear();  // Очищаем репер
    }

    function addAgentPosition(latitude, longitude) {
        // Очищаем предыдущую позицию агента, если она существует
        if (agentModel.count > 0) {
            agentModel.clear(); // Удаляем предыдущий маркер агента
        }

        // Добавляем новую позицию агента
        agentModel.append({ latitude: latitude, longitude: longitude });
        console.log("Agent position updated:", latitude, longitude);
    }
}
