#!/bin/bash
set -e

# -------------------------
# Определяем путь к проекту
# -------------------------
# Скрипт должен работать независимо от текущей директории запуска.
# По умолчанию считаем, что корень проекта находится на уровень выше папки docker/.
SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"
DEFAULT_PROJECT_ROOT="$(realpath "${SCRIPT_DIR}/..")"

# Можно переопределить исходную директорию проекта:
#   UMAS_GUI_DIR=/path/to/UMAS_GUI ./docker/run_docker.sh
PROJECT_ROOT="${UMAS_GUI_DIR:-$DEFAULT_PROJECT_ROOT}"

if [ ! -d "$PROJECT_ROOT" ]; then
    echo "ERROR: PROJECT_ROOT does not exist: $PROJECT_ROOT" >&2
    exit 1
fi

# -------------------------
# Настройки
# -------------------------
# CONTAINER_NAME="qt_ros2"
# IMAGE_NAME="qt-ros2"

IMAGE_NAME="${DOCKER_IMAGE:-svvyppy/umas_gui:latest}"  # можно переопределить через переменную
CONTAINER_NAME="umas_gui_app"
# NETWORK_NAME="ros2-net"
NETWORK_NAME="host"


# ROS2 параметры
ROS_DOMAIN_ID=1

# -------------------------
# Создаём сеть, если её нет
# -------------------------
if ! docker network ls | grep -q "$NETWORK_NAME"; then
    echo "Creating docker network: $NETWORK_NAME"
    docker network create $NETWORK_NAME
fi

# -------------------------
# Даем root доступ к X
# -------------------------
echo "Giving root access to X server..."
xhost +si:localuser:root

# -------------------------
# Проверяем существующий контейнер
# -------------------------
if [ "$(docker ps -a -q -f name=^/${CONTAINER_NAME}$)" ]; then
    echo "Container '$CONTAINER_NAME' already exists. Removing..."
    docker rm -f $CONTAINER_NAME
fi

# -------------------------
# Запуск контейнера
# -------------------------
docker run -it \
    --name $CONTAINER_NAME \
    --network $NETWORK_NAME \
    --privileged \
    -e DISPLAY=$DISPLAY \
    -e ROS_DOMAIN_ID=$ROS_DOMAIN_ID \
    -v /tmp/.X11-unix:/tmp/.X11-unix \
    -v "${PROJECT_ROOT}:/UMAS_GUI:rw" \
    $IMAGE_NAME \
    /bin/bash

# -------------------------
# Отзываем доступ к X
# -------------------------
echo "Revoking root access to X server..."
xhost -si:localuser:root
