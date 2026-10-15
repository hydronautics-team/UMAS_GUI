# Сборка и запуск

## Команда `umas-gui`

Один раз установите команду для текущего клона репозитория:

```bash
./scripts/install-cli.sh
```

Если `~/.local/bin` уже находится в `PATH`, после этого доступны команды:

```bash
umas-gui build   # собрать Docker-образ и приложение
umas-gui run     # запустить пульт
umas-gui shell   # открыть shell в контейнере
umas-gui format  # применить clang-format
umas-gui check   # проверить форматирование
```

Для `format` и `check` используется локальный `clang-format`, если он установлен.
Иначе CLI запускает инструмент в Docker и при необходимости обновляет образ.

Образ по умолчанию называется `qt-ros2`, а `ROS_DOMAIN_ID` равен `1`. Значения можно
переопределить переменными `UMAS_GUI_IMAGE` и `ROS_DOMAIN_ID`.

## Docker

Рекомендуемый способ работы с проектом — контейнер из `docker/Dockerfile`.

Все команды выполняются из корня репозитория.

1. Собрать образ:

   ```bash
   ./docker/build_docker.sh
   ```

2. Запустить контейнер с доступом к дисплею и устройствам:

   ```bash
   ./docker/run_docker.sh
   ```

3. Собрать приложение внутри контейнера:

   ```bash
   ./docker/compile_app.sh
   ```

4. Запустить приложение:

   ```bash
   ./docker/run_app.sh
   ```

## Локальная сборка

Для локальной сборки необходимы ROS 2 Humble, Qt 5, SFML и GStreamer.

Через colcon:

```bash
colcon build --packages-select UMAS_GUI
source install/setup.bash
```

Через CMake:

```bash
cmake -S . -B build
cmake --build build -j
```

## ROS 2

По умолчанию контейнер запускается с `ROS_DOMAIN_ID=1`. Изменить значение можно в `docker/run_docker.sh`.
