cmake_minimum_required(VERSION 3.5)

project(UMAS_GUI VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Charts)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui)



include_directories("${PROJECT_SOURCE_DIR}/compass")
include_directories("${PROJECT_SOURCE_DIR}/remote_control")
include_directories("${PROJECT_SOURCE_DIR}/uv")
include_directories("${PROJECT_SOURCE_DIR}/interface")
include_directories("${PROJECT_SOURCE_DIR}/map")
include_directories("${PROJECT_SOURCE_DIR}/communication")
include_directories("${PROJECT_SOURCE_DIR}/ui_utils")
include_directories("${PROJECT_SOURCE_DIR}/missions")
include_directories("${PROJECT_SOURCE_DIR}/tabs")
include_directories("${PROJECT_SOURCE_DIR}")

include_directories(${Python3_INCLUDE_DIRS})
#include_directories(${pybind11_INCLUDE_DIRS})


find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
#find_package(pybind11 REQUIRED)

include_directories(${Python3_INCLUDE_DIRS})
#include_directories(${pybind11_INCLUDE_DIRS})

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        remote_control/remote_control.h
        remote_control/remote_control.cpp
        compass/compass.cpp
        compass/compass.h
        compass/compass.ui
        uv/uv_state.cpp
        uv/uv_state.h
        interface/i_control_data.cpp
        interface/i_control_data.h
        interface/i_basic_data.cpp
        interface/i_basic_data.h
        interface/i_user_interface_data.cpp
        interface/i_user_interface_data.h
        interface/i_server_data.h
        interface/i_server_data.cpp
        communication/pc_protocol.h
        communication/udp_protocol.h
        ui_utils/setup_imu.h
        ui_utils/setup_imu.cpp
        ui_utils/setup_imu.ui
        ui_utils/setupimu_start.h
        ui_utils/setupimu_start.cpp
        ui_utils/setupimu_start.ui
        map/map.h
        map/map.cpp
        map/map.ui
        img.qrc
        ui_utils/setupimu_check.h
        ui_utils/setupimu_check.cpp
        ui_utils/setupimu_check.ui
        ui_utils/database.h
        ui_utils/database.cpp
        remote_control/key_board.h remote_control/key_board.cpp
        remote_control/joy_stick.h remote_control/joy_stick.cpp
        tabs/power_system.h tabs/power_system.cpp tabs/power_system.ui
        tabs/check_msg.h tabs/check_msg.cpp tabs/check_msg.ui
        tabs/check_imu.h tabs/check_imu.cpp tabs/check_imu.ui
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

target_link_libraries(UMAS_GUI PRIVATE
    sfml-graphics sfml-window sfml-system
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Charts
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Gui
    Python3::Python
#    pybind11::module
    )




set_target_properties(UMAS_GUI PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS UMAS_GUI
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(UMAS_GUI)
endif()
