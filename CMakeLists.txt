cmake_minimum_required(VERSION 3.20)
project(CrossingGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ----------------------------------------------------------------
# SFML Dependency Configuration (macOS Homebrew + Windows VS)
# ----------------------------------------------------------------
# If SFML_DIR or SFML_ROOT is set in environment or cmake arguments, search there first:
if(DEFINED ENV{SFML_DIR})
    list(PREPEND CMAKE_PREFIX_PATH "$ENV{SFML_DIR}")
endif()
if(DEFINED ENV{SFML_ROOT})
    list(PREPEND CMAKE_PREFIX_PATH "$ENV{SFML_ROOT}")
endif()

find_package(SFML 3 COMPONENTS Graphics Window System Audio QUIET)

if(NOT SFML_FOUND)
    find_package(SFML COMPONENTS Graphics Window System Audio REQUIRED)
endif()

# ----------------------------------------------------------------
# Include Directories
# ----------------------------------------------------------------
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils
    ${CMAKE_CURRENT_SOURCE_DIR}/CPeople
    ${CMAKE_CURRENT_SOURCE_DIR}/CVehicle
    ${CMAKE_CURRENT_SOURCE_DIR}/CAnimal
    ${CMAKE_CURRENT_SOURCE_DIR}/CGame
)

# ----------------------------------------------------------------
# Source Files
# ----------------------------------------------------------------
set(SOURCES
    Main/MainProgram.cpp
    CGame/CGame.cpp
    CVehicle/CVehicle.cpp
    CPeople/CPeople.cpp
    CAnimal/CAnimal.cpp
    Utils/ProfileManager.cpp
)

add_executable(CrossingGame ${SOURCES})

# ----------------------------------------------------------------
# Link Libraries
# ----------------------------------------------------------------
if(TARGET SFML::Graphics)
    target_link_libraries(CrossingGame PRIVATE
        SFML::Graphics
        SFML::Window
        SFML::System
        SFML::Audio
    )
else()
    target_link_libraries(CrossingGame PRIVATE
        ${SFML_LIBRARIES}
        ${SFML_DEPENDENCIES}
    )
endif()

# ----------------------------------------------------------------
# Set Working Directory for CLion / Visual Studio Run/Debug
# ----------------------------------------------------------------
set_target_properties(CrossingGame PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
