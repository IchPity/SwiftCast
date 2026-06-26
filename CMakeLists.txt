cmake_minimum_required(VERSION 3.15)
project(SwiftCast
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "High-performance productivity launcher"
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network Gui)

include_directories(${PROJECT_SOURCE_DIR}/src)

set(SOURCES
    src/main.cpp
    src/core/Application.cpp
    src/core/HotkeyManager.cpp
    src/core/Settings.cpp
    src/core/Updater.cpp
    src/ui/LauncherWindow.cpp
    src/ui/HotkeyEditor.cpp
    src/ui/SettingsDialog.cpp
    src/extensions/ExtensionLoader.cpp
    src/extensions/ExtensionStore.cpp
    src/utils/FileSystem.cpp
)

set(HEADERS
    src/core/Application.h
    src/core/HotkeyManager.h
    src/core/Settings.h
    src/core/Updater.h
    src/ui/LauncherWindow.h
    src/ui/HotkeyEditor.h
    src/ui/SettingsDialog.h
    src/extensions/ExtensionBase.h
    src/extensions/ExtensionLoader.h
    src/extensions/ExtensionStore.h
    src/utils/FileSystem.h
)

set(RESOURCES
    resources/resources.qrc
)

if(WIN32)
    set(APP_ICON resources/icons/logo.ico)
    set(SOURCES ${SOURCES} resources/windows/app.rc)
endif()

add_executable(swiftcast WIN32 ${SOURCES} ${HEADERS} ${RESOURCES})

target_link_libraries(swiftcast PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    Qt6::Gui
)

if(WIN32)
    target_link_libraries(swiftcast PRIVATE user32 dwmapi shell32 shlwapi)
endif()

install(TARGETS swiftcast RUNTIME DESTINATION bin)
install(DIRECTORY resources/styles DESTINATION bin/resources)

enable_testing()

add_executable(test_settings tests/test_settings.cpp)
target_link_libraries(test_settings PRIVATE Qt6::Core)
add_test(NAME SettingsPersistence COMMAND test_settings)

set_target_properties(swiftcast PROPERTIES
    OUTPUT_NAME "SwiftCast"
    WIN32_EXECUTABLE TRUE
)
