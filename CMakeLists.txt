cmake_minimum_required(VERSION 3.20)
project(pf_imgui_sdl_template)

if (MSVC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.29.30129 AND CMAKE_VERSION VERSION_GREATER 3.20.3)
    set(CMAKE_CXX_STANDARD 23) # /std:c++latest - unlocks the non stable cpp20 features. For new 16.11 versions
else ()
    set(CMAKE_CXX_STANDARD 20) # /std:c++latest for msvc and -std=c++20 for everyone else.
endif ()

find_package(SDL2 CONFIG REQUIRED)
find_package(pf_common CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(range-v3 CONFIG REQUIRED)
find_package(tomlplusplus CONFIG REQUIRED)
find_package(pf_mainloop CONFIG REQUIRED)
find_package(pf_imgui CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_executable(pf_imgui_sdl_template
        src/main.cpp
        src/imgui/ImGuiSDLInterface.cpp
        src/imgui/impl/imgui_impl_sdl.cpp
        src/imgui/impl/imgui_impl_sdlrenderer.cpp
        src/utils/files.cpp
        src/utils/files.h)


target_link_libraries(pf_imgui_sdl_template SDL2::SDL2 SDL2::SDL2main pf_common::pf_common pf_imgui::pf_imgui pf_mainloop::pf_mainloop spdlog::spdlog)