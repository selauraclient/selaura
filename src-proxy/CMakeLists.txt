cmake_minimum_required(VERSION 3.28)
project(selaura_proxy LANGUAGES CXX)

add_library(selaura_proxy SHARED
        main_win.cpp
        vcruntime.cpp
        ../src/platform/windows.cpp
)

target_include_directories(selaura_proxy PRIVATE
        fmt
        EnTT
        spdlog
        libhat
        gsl
        glm
        safetyhook
        glaze
        magic_enum

        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(selaura_proxy PRIVATE
        EnTT::EnTT
        spdlog
        libhat
        Microsoft.GSL::GSL
        glm
        safetyhook::safetyhook
        glaze::glaze
        magic_enum::magic_enum
        runtimeobject.lib
        version.lib
        Dwmapi.lib
)