# Сборка и запуск

## Команда `umas-gui`

Один раз установите команду для текущего клона репозитория:

```bash
./scripts/install-cli.sh
```

Если `~/.local/bin` уже находится в `PATH`, после этого доступны команды:

```bash
umas-gui docker-build  # собрать или обновить Docker-образ
umas-gui build         # собрать только приложение
umas-gui run           # запустить пульт
umas-gui shell         # открыть shell в контейнере
umas-gui format        # применить clang-format
umas-gui check         # проверить форматирование
```

При первом запуске или после изменения `docker/Dockerfile` выполните
`umas-gui docker-build`. В повседневной разработке достаточно быстрой команды
`umas-gui build`: существующий Docker-образ при этом не пересобирается.

Для `format` и `check` используется локальный `clang-format`, если он установлен.
Иначе CLI запускает инструмент в готовом Docker-образе. Если образ ещё не собран
или устарел, команда подскажет выполнить `umas-gui docker-build`.

Образ по умолчанию называется `qt-ros2`, а `ROS_DOMAIN_ID` равен `1`. Значения можно
переопределить переменными `UMAS_GUI_IMAGE` и `ROS_DOMAIN_ID`.

## Docker

Команда `umas-gui docker-build` собирает образ из `docker/Dockerfile`. Команды
`umas-gui build`, `umas-gui run` и `umas-gui shell` используют готовый образ и
не запускают его повторную сборку. Все параметры контейнера централизованы в CLI.

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

По умолчанию контейнер запускается с `ROS_DOMAIN_ID=1`. Для другого домена передайте
переменную при запуске, например: `ROS_DOMAIN_ID=2 umas-gui run`.
