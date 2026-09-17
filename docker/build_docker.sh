#!/bin/bash
set -e

IMAGE_NAME="qt-ros2"

echo "Building Docker image: $IMAGE_NAME"
docker build -t $IMAGE_NAME .
echo "Docker image built successfully."
