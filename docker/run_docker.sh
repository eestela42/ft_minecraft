#!/bin/bash

# Get the directory where the script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
# Get the project root (parent of script dir)
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

# Build the docker image
echo "Building Docker image..."
docker build -t ft_minecraft -f "$SCRIPT_DIR/Dockerfile" "$PROJECT_ROOT"

# Run the container
# --gpus all: Pass the GPU to the container
# -e DISPLAY: Forward the display environment variable
# -v /tmp/.X11-unix:/tmp/.X11-unix: Mount the X11 socket
# -v "$PROJECT_ROOT":/app: Mount the project directory so changes persist
# --rm: Remove container after exit

echo "Running ft_minecraft in Docker..."
docker run --gpus all -it --rm \
    -e DISPLAY=$DISPLAY \
    -e WAYLAND_DISPLAY=$WAYLAND_DISPLAY \
    -e XDG_RUNTIME_DIR=$XDG_RUNTIME_DIR \
    -e PULSE_SERVER=$PULSE_SERVER \
    -v /tmp/.X11-unix:/tmp/.X11-unix \
    -v /mnt/wslg:/mnt/wslg \
    -v "$PROJECT_ROOT":/app \
    ft_minecraft


# For windows users using WSL2 and X11 server (like VcXsrv or Xming), uncomment the following lines and comment the above docker run command
# # Get the IP of the Windows host
# export DISPLAY=$(grep nameserver /etc/resolv.conf | awk '{print $2}'):0.0

# # Run without mounting /tmp/.X11-unix and without WAYLAND_DISPLAY
# docker run --gpus all -it --rm \
#     -e DISPLAY=$DISPLAY \
#     -v "$PROJECT_ROOT":/app \
#     ft_minecraft

