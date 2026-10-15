# Сборка и запуск

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
