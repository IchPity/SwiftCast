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

set(APP_SOURCES
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

set(SOURCES
    src/main.cpp
    ${APP_SOURCES}
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

# --- Windows: auto-deploy Qt runtime DLLs next to the executable ---
# Without this the .exe fails to start with "Qt6Network.dll not found" etc.,
# because the Qt runtime DLLs and plugins are not on the PATH at runtime.
if(WIN32)
    # windeployqt ships in the same bin/ dir as the Qt tools.
    get_target_property(_qmake_exe Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_exe}" DIRECTORY)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET swiftcast POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}"
                    --no-translations
                    --no-compiler-runtime
                    "$<IF:$<CONFIG:Debug>,--debug,--release>"
                    "$<TARGET_FILE:swiftcast>"
            COMMENT "Deploying Qt runtime DLLs with windeployqt"
            VERBATIM)
    else()
        message(WARNING "windeployqt not found - Qt DLLs will NOT be deployed automatically.")
    endif()
endif()

install(TARGETS swiftcast RUNTIME DESTINATION bin)
install(DIRECTORY resources/styles DESTINATION bin/resources)

enable_testing()

add_executable(test_settings tests/test_settings.cpp)
target_link_libraries(test_settings PRIVATE Qt6::Core)
add_test(NAME SettingsPersistence COMMAND test_settings)

# Manual UI launcher for development/testing — not a CTest test (requires user interaction)
add_executable(test_launch WIN32
    tests/test_launch.cpp
    ${APP_SOURCES}
    ${HEADERS}
    ${RESOURCES}
)
target_link_libraries(test_launch PRIVATE
    Qt6::Core Qt6::Widgets Qt6::Network Qt6::Gui
)
if(WIN32)
    target_link_libraries(test_launch PRIVATE user32 dwmapi shell32 shlwapi)
endif()

set_target_properties(swiftcast PROPERTIES
    OUTPUT_NAME "SwiftCast"
    WIN32_EXECUTABLE TRUE
)
