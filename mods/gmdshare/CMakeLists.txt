cmake_minimum_required(VERSION 3.3.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(gmdshare VERSION 1.0.0)

set_source_files_properties(src/file-dialog.mm PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
set(SOURCES
	src/main.cpp
	src/file-dialog.mm
)

include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)

CPMAddPackage(
	NAME base64
	GITHUB_REPOSITORY ReneNyffenegger/cpp-base64
	GIT_TAG V2.rc.08
	DOWNLOAD_ONLY YES
)

CPMAddPackage("gh:leethomason/tinyxml2#9.0.0")

list(APPEND SOURCES ${base64_SOURCE_DIR}/base64.cpp)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_include_directories(${PROJECT_NAME}
	PRIVATE
	$<BUILD_INTERFACE:${base64_SOURCE_DIR}>
)

target_link_libraries(${PROJECT_NAME} tinyxml2)

target_link_libraries(${PROJECT_NAME} geode-sdk)
create_geode_file(${PROJECT_NAME})
