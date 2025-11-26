#!/bin/bash
set -e

# -------------------------
# Настройки
# -------------------------
CONTAINER_NAME="qt_ros2"
IMAGE_NAME="qt-ros2"
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
    -v ~/Desktop/hydro/UMAS_GUI:/UMAS_GUI:rw \
    $IMAGE_NAME \
    /bin/bash

# -------------------------
# Отзываем доступ к X
# -------------------------
echo "Revoking root access to X server..."
xhost -si:localuser:root
