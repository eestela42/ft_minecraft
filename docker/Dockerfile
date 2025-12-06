FROM ubuntu:24.04

# Install build dependencies and libraries
RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    make \
    libglfw3-dev \
    libx11-dev \
    liblmdb-dev \
    xorg-dev \
    libglu1-mesa-dev \
    git \
    libglvnd-dev \
    libgl1 \
    libglx0 \
    libegl1 \
    libgles2 \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy the current directory contents into the container
COPY . /app

# Build the project
RUN make clean && make

# Command to run the application
# We use the environment variables we found earlier to ensure compatibility
CMD ["sh", "-c", "MESA_GL_VERSION_OVERRIDE=4.6COMPAT MESA_GLSL_VERSION_OVERRIDE=460 ./ft_minecraft"]
