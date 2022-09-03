cmake_minimum_required(VERSION 3.3.0)

set(CMAKE_OSX_ARCHITECTURES "x86_64" CACHE INTERNAL "" FORCE)

project(zmxmods VERSION 0.1.0)

add_custom_target(zmxmods)

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

set(GEODE_LINK_SOURCE On)
add_subdirectory($ENV{GEODE_SDK} $ENV{GEODE_SDK}/build)

add_subdirectory(mods)
