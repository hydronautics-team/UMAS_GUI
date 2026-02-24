# UMAS_GUI

Проект — операторский пульт управления подводными аппаратами. Qt-приложение, которое взаимодействует с бортовой системой через ROS 2: получает телеметрию/видео, визуализирует состояние и отправляет управляющие команды.

Пример GUI при планировании миссии
Добавьте изображение по пути: gui.png

Архитектура взаимодействия с ROS 2
- RosBridge (ros2_bridge/RosBridge) работает в отдельном QThread и инициализирует rclcpp::Node с namespace "qt_controller".
- Публикации:
  - geometry_msgs::msg::Twist → /control/data (управляющие команды)
  - std_msgs::msg::UInt8 → /control/loop_flags (флаги режимов управления)

Сборка и запуск
Рекомендуется использовать контейнеры, скрипты уже подготовлены в ./docker:

1) Собрать образ:
```bash
# Bash
./docker/build_docker.sh
```

2) Запустить контейнер (среда выполнения):
```bash
# Bash
./docker/run_docker.sh
```

3) Скомпилировать приложение (в контейнере ):
```bash
# Bash
./docker/compile_app.sh
```

4) Запустить приложение:
```bash
# Bash
./docker/run_app.sh
```

Альтернативно (локально, при наличии ROS 2 и Qt):
- В рабочей директории ROS 2 workspace:
```bash
# Bash
colcon build --packages-select UMAS_GUI
source install/setup.bash
# или стандартный CMake-подход:
mkdir build && cd build
cmake .. && make -j$(nproc)
```

Структура проекта (ключевые папки/файлы)
- CMakeLists.txt, package.xml — сборка и зависимости
- docker/ — скрипты сборки и запуска
- main.cpp, mainwindow.* — точка входа и главный UI
- ros2_bridge/ — RosBridge (rclcpp ↔ Qt)
- input/ — адаптеры источников ввода (gamepad, клавиатура)
- remote_control/ — joystick/keyboard helpers
- control/ — control_service (применение команд)
- uv/ — UVState (центральная модель состояния аппарата)
- compass/, map/, Diagnostic_bord_UI/ — дополнительные UI-модули
- Gamepad/, mods/ — вспомогательные модули и режимы

