cmake_minimum_required(VERSION 3.16)
project(joy_monitor_imgui LANGUAGES CXX)
 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
 
include(FetchContent)
 
# --- SDL3 --------------------------------------------------------------
FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.2.8
    GIT_SHALLOW TRUE
)
# We only need the library, not SDL's own tests/examples.
set(SDL_TEST OFF CACHE BOOL "" FORCE)
set(SDL_SHARED ON CACHE BOOL "" FORCE)
set(SDL_STATIC OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(SDL3)
 
# --- Dear ImGui (just the sources, no CMake of its own) ----------------
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.93.0-docking
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(imgui)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBEVDEV REQUIRED libevdev)
 
add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui PUBLIC SDL3::SDL3)
 
# --- find a GL implementation -------------------------------------------
find_package(OpenGL REQUIRED)
 
# --- our executable -------------------------------------------------------
add_executable(hands_on
    src/main.cpp
    src/joystick.cpp
    src/vjoystick.cpp
    src/sdl_joystick.cpp
    src/input.cpp
    src/ui.cpp
    src/json.cpp
    src/context.cpp
)
target_link_libraries(hands_on PRIVATE
    imgui
    SDL3::SDL3
    OpenGL::GL
    evdev
    ${LIBEVDEV_LIBRARIES}
    ${LIBEVDEV_LDFLAGS}
)

target_include_directories(hands_on PRIVATE ${LIBEVDEV_INCLUDE_DIRS})
 
# Copy SDL3's shared lib next to the exe on Windows so it runs without
# fiddling with PATH; harmless no-op on Linux/macOS.
if (WIN32)
    add_custom_command(TARGET joy_monitor_imgui POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3>
            $<TARGET_FILE_DIR:joy_monitor_imgui>
    )
endif()
