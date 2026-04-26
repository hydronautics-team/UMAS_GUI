#!/bin/bash
set -e

IMAGE_NAME="${DOCKER_IMAGE:-hydronautics/umas-gui}"

echo "Building Docker image: $IMAGE_NAME"
docker build -t "$IMAGE_NAME" .
echo "Docker image built successfully."
