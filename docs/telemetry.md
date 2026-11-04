# Контракт дополнительной телеметрии

Карточки верхней панели получают независимые потоки ROS 2. Отсутствие или устаревание одного
потока не делает остальные карточки невалидными.

| Параметр узла | Топик или сервис по умолчанию | Тип | Карточка |
| --- | --- | --- | --- |
| `imu_topic` | `/imu/data` | `sensor_msgs/msg/Imu` | Ориентация и авиагоризонт |
| `temperature_topic` | `/ms5837/temperature` | `std_msgs/msg/Float32` | ТЕМП |
| `bottom_distance_topic` | `/stingray_core/sensors/dvl/altitude` | `std_msgs/msg/Float32` | ДО ДНА |
| `leak_state_topic` | `/stingray_core/safety/leak` | `std_msgs/msg/Bool` | ПРОТЕЧКА |
| `kill_switch_state_topic` | `/stingray_core/safety/kill_switch` | `std_msgs/msg/Bool` | КИЛЛСВИЧ |
| `health_ping_service` | `/stingray_core/health/ping` | `std_srvs/srv/Trigger` | ПИНГ |

Имена источников можно переопределить параметрами узла `/qt_controller/qt_controller_node`.
Значения `NaN` и бесконечность игнорируются; расстояние до дна также не может быть отрицательным.
Если поток не обновлялся три секунды, соответствующая карточка показывает `N/A`.

Топики безопасности используют QoS `reliable + transient local`, чтобы новый экземпляр GUI сразу
получал последнее состояние. Публикующая сторона должна использовать совместимый QoS.
