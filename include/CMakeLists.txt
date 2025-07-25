cmake_minimum_required(VERSION 3.28)
project(selaura LANGUAGES CXX)

if(NOT CPM_SOURCE_CACHE)
    set(CPM_SOURCE_CACHE "${CMAKE_BINARY_DIR}/.cpmcache")
endif()

if(NOT EXISTS "${CMAKE_BINARY_DIR}/cmake/CPM.cmake")
    file(DOWNLOAD
            https://github.com/cpm-cmake/CPM.cmake/releases/latest/download/CPM.cmake
            "${CMAKE_BINARY_DIR}/cmake/CPM.cmake"
            TLS_VERIFY ON)
endif()
include("${CMAKE_BINARY_DIR}/cmake/CPM.cmake")

CPMAddPackage(
        NAME fmt
        GITHUB_REPOSITORY fmtlib/fmt
        GIT_TAG master
)

CPMAddPackage(
        NAME entt
        GITHUB_REPOSITORY skypjack/entt
        GIT_TAG d4014c74dc3793aba95ae354d6e23a026c2796db
)

CPMAddPackage(
        NAME libhat
        GITHUB_REPOSITORY BasedInc/libhat
        GIT_TAG master
)

CPMAddPackage(
        NAME spdlog
        GITHUB_REPOSITORY gabime/spdlog
        GIT_TAG v1.x
)

CPMAddPackage(
        NAME gsl
        GITHUB_REPOSITORY microsoft/GSL
        GIT_TAG v4.0.0
)

CPMAddPackage(
        NAME glm
        GITHUB_REPOSITORY g-truc/glm
        GIT_TAG master
)

CPMAddPackage(
        NAME safetyhook
        GITHUB_REPOSITORY cursey/safetyhook
        GIT_TAG main
)

CPMAddPackage(
        NAME glaze
        GITHUB_REPOSITORY stephenberry/glaze
        GIT_TAG main
)

CPMAddPackage(
        NAME magic_enum
        GITHUB_REPOSITORY Neargye/magic_enum
        GIT_TAG master
)

add_library(selaura INTERFACE)
add_library(selaura::selaura ALIAS selaura)

target_include_directories(selaura INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(selaura INTERFACE
        fmt
        EnTT::EnTT
        libhat
        spdlog::spdlog
        GSL::gsl
        glm
        safetyhook::safetyhook
        glaze::glaze
        magic_enum::magic_enum
)