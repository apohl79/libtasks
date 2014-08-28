project(http_client)
cmake_minimum_required(VERSION 2.6)

aux_source_directory(. SOURCES)
#add_compile_options(-O4 -ggdb -std=c++11 -pthread)
add_definitions(-O4 -ggdb -std=c++1y -pthread)

include_directories(${PROJECT_SOURCE_DIR}/../../include ${PROJECT_SOURCE_DIR})

link_directories("${CMAKE_BINARY_DIR}/tasks")

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} tasks ev thrift)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
  add_definitions(-D_DEBUG_OUTPUT -DLOGMUTEX)
endif(CMAKE_BUILD_TYPE MATCHES "Debug")

if(WITH_PROFILER MATCHES "y" OR WITH_PROFILER MATCHES "Y")
  add_definitions(-DPROFILER)
  target_link_libraries(${PROJECT_NAME} profiler)
endif(WITH_PROFILER MATCHES "y" OR WITH_PROFILER MATCHES "Y")
