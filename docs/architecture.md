# Архитектура UMAS GUI

## Взаимодействие с ROS 2

`RosBridge` расположен в `src/integrations/ros` и работает в отдельном `QThread`. Он связывает Qt-интерфейс с узлами `stingray_core`.

Основные входящие данные:

- `/core/state/odometry` — положение, ориентация и скорость;
- `/stingray_core/battery/state` — состояние аккумуляторов.

Основные исходящие команды:

- `/control/data` (`geometry_msgs/msg/Twist`) — управляющие воздействия;
- `/control/loop_flags` (`std_msgs/msg/UInt8`) — флаги контуров управления.

## Структура проекта

```text
src/
├── app/                 # точка входа и главное окно
├── core/                # состояние и прикладная логика
├── integrations/        # ROS, видео и устройства ввода
├── ui/                  # приборы и панели интерфейса
└── features/            # карта и отдельные режимы

resources/               # изображения и Qt-ресурсы
docker/                  # Dockerfile и скрипты контейнера
docs/                    # документация
```

`Diagnostic_bord_UI` и `algorithm_cpp` подключены как внешние submodule и пока сохраняют собственную структуру.
