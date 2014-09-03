project(http_client)
cmake_minimum_required(VERSION 2.6)
include(CMakeBase)

aux_source_directory(. SOURCES)

include_directories(${PROJECT_SOURCE_DIR}/../../include ${PROJECT_SOURCE_DIR})

link_directories("${CMAKE_BINARY_DIR}/tasks")

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} tasks ev)
