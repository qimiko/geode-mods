cmake_minimum_required(VERSION 3.3.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(highres VERSION 1.0.0)

set_source_files_properties(src/main.mm PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
set(SOURCES
	src/main.mm
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_link_libraries(${PROJECT_NAME} geode-sdk)
create_geode_file(${PROJECT_NAME})
