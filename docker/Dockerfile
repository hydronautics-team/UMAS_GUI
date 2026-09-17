# Dockerfile: Qt5 + ROS2 Humble (Ubuntu 22.04) — исправлено имя пакета serialport
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive
ENV LANG=en_US.UTF-8
ENV LC_ALL=en_US.UTF-8

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    git \
    wget \
    curl \
    lsb-release \
    gnupg2 \
    locales \
    python3-pip \
    python3-venv \
    python3-setuptools \
    ca-certificates \
    software-properties-common \
    && rm -rf /var/lib/apt/lists/*

RUN locale-gen en_US en_US.UTF-8

# colcon и утилиты через pip (надёжнее в контейнере)
RUN pip3 install --no-cache-dir colcon-common-extensions rosdep vcstool rosinstall-generator

RUN rosdep init || true

# Добавляем ключ ROS (apt-key используется для простоты в контейнере)
RUN curl -sSL https://raw.githubusercontent.com/ros/rosdistro/master/ros.asc | apt-key add -

# Включаем universe (нужно для ряда Qt-пакетов)
RUN add-apt-repository universe

# Добавляем репозиторий ROS2
RUN add-apt-repository "deb http://packages.ros.org/ros2/ubuntu $(lsb_release -cs) main"

RUN apt-get update && apt-get install -y --no-install-recommends \
    ros-humble-desktop \
    && rm -rf /var/lib/apt/lists/*
    
RUN apt-get update && apt-get install -y \
    libsfml-dev

RUN echo "source /opt/ros/humble/setup.bash" >> /etc/bash.bashrc

# ---------------------------------------------------------
# Qt5 dev packages (исправлено имя пакета serialport)
# ---------------------------------------------------------
RUN apt-get update && apt-get install -y --no-install-recommends \
    qtbase5-dev \
    qtchooser \
    qt5-qmake \
    qtbase5-dev-tools \
    qtdeclarative5-dev \
    qttools5-dev \
    qttools5-dev-tools \
    qtmultimedia5-dev \
    qtpositioning5-dev \
    qtlocation5-dev \
    libqt5serialport5-dev \
    qtquickcontrols2-5-dev \
    qml-module-qtquick-controls2 \
    qml-module-qtgraphicaleffects \
    qml-module-qtquick2 \
    qml-module-qtquick-layouts \
    qml-module-qtquick-window2 \
    qml-module-qt-labs-platform \
    libqt5charts5-dev \
    && rm -rf /var/lib/apt/lists/*

# Workspace
WORKDIR /UMAS_GUI

# копируем код
COPY . .

# сборка
RUN /bin/bash -c "source /opt/ros/humble/setup.bash && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make -j$(nproc)"

CMD ["/bin/bash"]
