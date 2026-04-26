#!/bin/bash
set -e

CONTAINER_NAME="${CONTAINER_NAME:-umas_gui}"
IMAGE_NAME="${DOCKER_IMAGE:-hydronautics/umas-gui}"
NETWORK_NAME="${DOCKER_NETWORK:-host}"
ROS_DOMAIN_ID="${ROS_DOMAIN_ID:-1}"
ROS_LOCALHOST_ONLY="${ROS_LOCALHOST_ONLY:-0}"
RMW_IMPLEMENTATION="${RMW_IMPLEMENTATION:-rmw_fastrtps_cpp}"


if [ "$NETWORK_NAME" != "host" ] && ! docker network ls | grep -q "$NETWORK_NAME"; then
    echo "Creating docker network: $NETWORK_NAME"
    docker network create "$NETWORK_NAME"
fi

echo "Giving root access to X server..."
xhost +si:localuser:root
trap 'echo "Revoking root access to X server..."; xhost -si:localuser:root >/dev/null 2>&1 || true' EXIT

if [ "$(docker ps -a -q -f name=^/${CONTAINER_NAME}$)" ]; then
    echo "Container '$CONTAINER_NAME' already exists. Removing..."
    docker rm -f "$CONTAINER_NAME"
fi

docker run -it --rm \
    --name "$CONTAINER_NAME" \
    --network "$NETWORK_NAME" \
    --ipc host \
    --privileged \
    -e DISPLAY="$DISPLAY" \
    -e ROS_DOMAIN_ID="$ROS_DOMAIN_ID" \
    -e ROS_LOCALHOST_ONLY="$ROS_LOCALHOST_ONLY" \
    -e RMW_IMPLEMENTATION="$RMW_IMPLEMENTATION" \
    -v /tmp/.X11-unix:/tmp/.X11-unix \
    "$IMAGE_NAME" \
    "$@"
