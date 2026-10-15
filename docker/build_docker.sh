#!/bin/bash
set -e

IMAGE_NAME="qt-ros2"
SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"
PROJECT_ROOT="$(realpath "${SCRIPT_DIR}/..")"

echo "Building Docker image: $IMAGE_NAME"
docker build -f "${SCRIPT_DIR}/Dockerfile" -t "$IMAGE_NAME" "$PROJECT_ROOT"
echo "Docker image built successfully."
