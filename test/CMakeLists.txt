cmake_minimum_required(VERSION 3.28)
project(selaura_test_mod LANGUAGES CXX)

add_library(selaura_test_mod SHARED
        main.cpp
        imgui/imgui.cpp
        imgui/imgui_demo.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
)

target_include_directories(selaura_test_mod PRIVATE
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

target_link_libraries(selaura_test_mod PRIVATE
        selaura_runtime
        EnTT::EnTT
        spdlog
        libhat
        Microsoft.GSL::GSL
        glm
        safetyhook::safetyhook
        glaze::glaze
        magic_enum::magic_enum
        runtimeobject.lib
)