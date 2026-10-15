cmake_minimum_required(VERSION 3.5)

project(UMAS_GUI VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(QT NAMES Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
# find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Charts)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui)
# find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS WebEngineWidgets)
# find_package(Qt5 REQUIRED COMPONENTS Widgets Quick QuickControls2 Positioning Location)
find_package(Qt5 REQUIRED COMPONENTS Widgets Quick )
find_package(Qt5 REQUIRED COMPONENTS Core  )
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(ament_cmake REQUIRED)


# GSTREAMER (добавлено)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GST REQUIRED gstreamer-1.0 gstreamer-app-1.0)

# Если pkg-config не нашел, используем ручные пути (из вывода pkg-config)
if(NOT GST_FOUND)
    message(WARNING "GStreamer not found via pkg-config, using manual paths")
    set(GST_INCLUDE_DIRS 
        "/usr/include/gstreamer-1.0"
        "/usr/include/x86_64-linux-gnu"
        "/usr/include/glib-2.0"
        "/usr/lib/x86_64-linux-gnu/glib-2.0/include"
    )
    set(GST_LIBRARIES 
        gstreamer-1.0
        gstapp-1.0
        gobject-2.0
        glib-2.0
    )
endif()

include_directories(${GST_INCLUDE_DIRS})
link_directories(${GST_LIBRARY_DIRS})

# include_directories(/opt/ros/humble/include)

include_directories("${PROJECT_SOURCE_DIR}/src/app")
include_directories("${PROJECT_SOURCE_DIR}/src/core/telemetry")
include_directories("${PROJECT_SOURCE_DIR}/src/core/control")
include_directories("${PROJECT_SOURCE_DIR}/src/integrations/ros")
include_directories("${PROJECT_SOURCE_DIR}/src/integrations/video")
include_directories("${PROJECT_SOURCE_DIR}/src/integrations/input")
include_directories("${PROJECT_SOURCE_DIR}/src/integrations/input/gamepad")
include_directories("${PROJECT_SOURCE_DIR}/src/integrations/input/legacy")
include_directories("${PROJECT_SOURCE_DIR}/src/ui/instruments/attitude")
include_directories("${PROJECT_SOURCE_DIR}/src/ui/instruments/compass")
include_directories("${PROJECT_SOURCE_DIR}/src/ui/instruments/depth")
include_directories("${PROJECT_SOURCE_DIR}")

include_directories(${Python3_INCLUDE_DIRS})


find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
# find_package(Qt5 REQUIRED COMPONENTS Widgets Quick QuickWidgets Positioning Location)
find_package(Qt5 REQUIRED COMPONENTS Widgets )

include_directories(${Python3_INCLUDE_DIRS})

set(PROJECT_SOURCES
        src/integrations/input/gamepad/gamepad.cpp
        src/integrations/input/gamepad/gamepad.h
        src/integrations/input/gamepad_input_source.h
        src/integrations/input/gamepad_input_source.cpp
        src/app/main.cpp
        src/app/mainwindow.cpp
        src/app/mainwindow.h
        src/app/mainwindow.ui
        resources/resources.qrc
        src/ui/instruments/attitude/attitudeindicator.h
        src/ui/instruments/attitude/attitudeindicator.cpp
        src/ui/instruments/compass/compass.cpp
        src/ui/instruments/compass/compass.h
        src/ui/instruments/compass/compass.ui
        src/ui/instruments/depth/depth_gauge.cpp
        src/ui/instruments/depth/depth_gauge.h
        src/core/telemetry/uv_state.cpp
        src/core/telemetry/uv_state.h
        src/integrations/input/i_input_source.h
        src/core/control/control_service.h
        src/integrations/input/legacy/key_board.h src/integrations/input/legacy/key_board.cpp
        src/integrations/input/legacy/joy_stick.h src/integrations/input/legacy/joy_stick.cpp
        src/integrations/ros/ros2_bridge.h
        src/integrations/ros/ros2_bridge.cpp
        src/integrations/video/video_player_widget.h
        src/integrations/video/video_player_widget.cpp
        src/integrations/video/fullscreen_video_window.cpp
        src/integrations/video/fullscreen_video_window.h
        )



if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(UMAS_GUI
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET UMAS_GUI APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(UMAS_GUI SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(UMAS_GUI
            ${PROJECT_SOURCES}
        )
    endif()
endif()

ament_target_dependencies(UMAS_GUI rclcpp geometry_msgs std_msgs sensor_msgs nav_msgs)

target_link_libraries(UMAS_GUI
    ${GST_LIBRARIES}                           # <-- ДОБАВЛЕНО
    sfml-graphics sfml-window sfml-system
    Qt${QT_VERSION_MAJOR}::Widgets
    # Qt${QT_VERSION_MAJOR}::Charts
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    # Qt${QT_VERSION_MAJOR}::WebEngineWidgets
    # Qt${QT_VERSION_MAJOR}::Positioning
    Python3::Python
)


# target_link_libraries(UMAS_GUI PRIVATE Qt5::Core Qt5::Widgets Qt5::Location)


set_target_properties(UMAS_GUI PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)
# ament_package()

install(TARGETS UMAS_GUI
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(UMAS_GUI)
endif()
