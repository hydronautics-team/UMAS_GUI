cmake_minimum_required(VERSION 3.5)

project(UMAS_GUI VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
    Widgets
    Charts
    Core
    Network
    Sql
    Gui
    WebEngineWidgets
    Positioning
)
find_package(Qt5 REQUIRED COMPONENTS Quick QuickControls2 Location)
find_package(X11 REQUIRED)

# === GStreamer ===
find_package(PkgConfig REQUIRED)
pkg_check_modules(GST REQUIRED
    gstreamer-1.0
    gstreamer-video-1.0
    gstreamer-app-1.0
)

# === Python конфигурация ===
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)



# Добавляем пути к заголовочным файлам GStreamer
include_directories(${GST_INCLUDE_DIRS})
include_directories(${Python3_INCLUDE_DIRS})
# Пути к вашим собственным заголовочным файлам
include_directories("${PROJECT_SOURCE_DIR}/compass")
include_directories("${PROJECT_SOURCE_DIR}/remote_control")
include_directories("${PROJECT_SOURCE_DIR}/uv")
include_directories("${PROJECT_SOURCE_DIR}/interface")
include_directories("${PROJECT_SOURCE_DIR}/map")
include_directories("${PROJECT_SOURCE_DIR}/communication")
include_directories("${PROJECT_SOURCE_DIR}/ui_utils")
include_directories("${PROJECT_SOURCE_DIR}/missions")
include_directories("${PROJECT_SOURCE_DIR}/tabs")
include_directories("${PROJECT_SOURCE_DIR}/mods")
include_directories("${PROJECT_SOURCE_DIR}/Diagnostic_bord_UI/Diagnostic_bord")
include_directories("${PROJECT_SOURCE_DIR}/videowidget")
include_directories("${PROJECT_SOURCE_DIR}")


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
        mods/mode_automatic.h mods/mode_automatic.cpp mods/mode_automatic.ui
        map/map_widget.h map/map_widget.cpp map/map_widget.ui map/map.html
        Diagnostic_bord_UI/Diagnostic_bord/diagnostic_board.cpp
        Diagnostic_bord_UI/Diagnostic_bord/diagnostic_board.h
        Diagnostic_bord_UI/Diagnostic_bord/diagnostic_board.ui
        videowidget/videowidget.cpp
        videowidget/videowidget.h
        videowidget/videowidget.ui
        )

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(UMAS_GUI
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(UMAS_GUI SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(UMAS_GUI
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# Подключаем все библиотеки в одном месте
target_link_libraries(UMAS_GUI PRIVATE
    sfml-graphics sfml-window sfml-system
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Charts
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::WebEngineWidgets
    Qt${QT_VERSION_MAJOR}::Positioning
    Python3::Python
    ${GST_LIBRARIES} # <-- КЛЮЧЕВОЕ ИЗМЕНЕНИЕ: подключаем GStreamer и все его зависимости
    ${X11_LIBRARIES}
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
