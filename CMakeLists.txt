cmake_minimum_required(VERSION 3.20)
include("cmake/HunterGate.cmake")

set(CPM_DOWNLOAD_LOCATION "${CMAKE_SOURCE_DIR}/cmake/CPM.cmake")
set(CPM_DOWNLOAD_VERSION 0.32.2)

message(https://github.com/TheLartians/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake)
if (NOT (EXISTS ${CPM_DOWNLOAD_LOCATION} AND CPM_VERSION STREQUAL CPM_DOWNLOAD_VERSION))
    message(STATUS "Downloading CPM.cmake")
    file(DOWNLOAD https://github.com/TheLartians/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake ${CPM_DOWNLOAD_LOCATION})
endif ()

include(${CPM_DOWNLOAD_LOCATION})


HunterGate(
        URL "https://github.com/cpp-pm/hunter/archive/v0.23.321.tar.gz"
        SHA1 "5e53cbb0429037ea8e2592bfd92704b8ff3ab492"
)


project(pf_imgui_sdl_template)

if (MSVC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.29.30129 AND CMAKE_VERSION VERSION_GREATER 3.20.3)
    set(CMAKE_CXX_STANDARD 23) # /std:c++latest - unlocks the non stable cpp20 features. For new 16.11 versions
else ()
    set(CMAKE_CXX_STANDARD 20) # /std:c++latest for msvc and -std=c++20 for everyone else.
endif ()

add_compile_definitions(FMT_HEADER_ONLY )

CPMAddPackage(
        NAME pf_common
        GITHUB_REPOSITORY PetrFlajsingr/pf_common
        GIT_TAG master
)

CPMAddPackage(
        NAME pf_mainloop
        GITHUB_REPOSITORY PetrFlajsingr/pf_mainloop
        GIT_TAG master
)
CPMAddPackage(
        NAME pf_imgui
        GITHUB_REPOSITORY PetrFlajsingr/pf_imgui
        GIT_TAG master
        OPTIONS "ENABLE_STYLES ON"
)
add_compile_definitions(PF_IMGUI_ENABLE_STYLES)

find_package(SDL2 CONFIG REQUIRED)

add_executable(pf_imgui_sdl_template
        src/main.cpp
        ${pf_imgui_SOURCE_DIR}/src/pf_imgui/backends/ImGuiSDLInterface.cpp
        ${pf_imgui_SOURCE_DIR}/src/pf_imgui/backends/impl/imgui_impl_sdl.cpp
        ${pf_imgui_SOURCE_DIR}/src/pf_imgui/backends/impl/imgui_impl_sdlrenderer.cpp src/utils/files.cpp src/utils/files.h)


target_link_libraries(pf_imgui_sdl_template SDL2::SDL2 SDL2::SDL2main pf_common::pf_common pf_imgui::pf_imgui pf_mainloop::pf_mainloop #fmt::fmt spdlog::spdlog
        )