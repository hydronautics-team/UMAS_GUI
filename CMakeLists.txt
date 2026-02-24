cmake_minimum_required(VERSION 3.5)

project(UMAS_GUI VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
find_package(ament_cmake REQUIRED)

# include_directories(/opt/ros/humble/include)

include_directories("${PROJECT_SOURCE_DIR}/compass")
include_directories("${PROJECT_SOURCE_DIR}/remote_control")
include_directories("${PROJECT_SOURCE_DIR}/uv")
include_directories("${PROJECT_SOURCE_DIR}/input")
include_directories("${PROJECT_SOURCE_DIR}/control")
# include_directories("${PROJECT_SOURCE_DIR}/map")
include_directories("${PROJECT_SOURCE_DIR}/tabs")
include_directories("${PROJECT_SOURCE_DIR}/mods")
include_directories("${PROJECT_SOURCE_DIR}/Diagnostic_bord_UI/Diagnostic_bord")
include_directories("${PROJECT_SOURCE_DIR}/ros2_bridge")
include_directories("${PROJECT_SOURCE_DIR}")

include_directories(${Python3_INCLUDE_DIRS})


find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
# find_package(Qt5 REQUIRED COMPONENTS Widgets Quick QuickWidgets Positioning Location)
find_package(Qt5 REQUIRED COMPONENTS Widgets )

include_directories(${Python3_INCLUDE_DIRS})

set(PROJECT_SOURCES
        Gamepad/gamepad.cpp
        Gamepad/gamepad.h
        input/gamepad_input_source.h
        input/gamepad_input_source.cpp
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        compass/compass.cpp
        compass/compass.h
        compass/compass.ui
        uv/uv_state.cpp
        uv/uv_state.h
        input/i_input_source.h
        control/control_service.h
        img.qrc
        remote_control/key_board.h remote_control/key_board.cpp
        remote_control/joy_stick.h remote_control/joy_stick.cpp
        # tabs/power_system.h tabs/power_system.cpp tabs/power_system.ui
        # tabs/check_msg.h tabs/check_msg.cpp tabs/check_msg.ui
        # tabs/check_imu.h tabs/check_imu.cpp tabs/check_imu.ui
        # mods/mode_automatic.h mods/mode_automatic.cpp mods/mode_automatic.ui
        # map/map_widget.h map/map_widget.cpp map/map_widget.ui map/map.html
        Diagnostic_bord_UI/Diagnostic_bord/diagnostic_board.cpp
        Diagnostic_bord_UI/Diagnostic_bord/diagnostic_board.h
        Diagnostic_bord_UI/Diagnostic_bord/diagnostic_board.ui
        ros2_bridge/ros2_bridge.h
        ros2_bridge/ros2_bridge.cpp
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

ament_target_dependencies(UMAS_GUI rclcpp geometry_msgs)

target_link_libraries(UMAS_GUI
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
