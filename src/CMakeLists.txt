cmake_minimum_required(VERSION 3.28)
project(selaura_runtime LANGUAGES CXX)

add_library(selaura_runtime SHARED
        main.cpp
        platform/windows.cpp
        runtime.cpp
        hooks/hooks.cpp
)

target_compile_definitions(selaura_runtime PRIVATE SELAURA_RUNTIME_EXPORTS)

target_include_directories(selaura_runtime PRIVATE
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

target_link_libraries(selaura_runtime PRIVATE
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
)