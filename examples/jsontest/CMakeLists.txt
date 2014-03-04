project(jsontest)
cmake_minimum_required(VERSION 2.6)

set(DEBUG 0)
set(PROFILER 0)

aux_source_directory(. SOURCES)
#add_compile_options(-O4 -ggdb -std=c++11 -pthread)
add_definitions(-O4 -ggdb -std=c++11 -pthread)

include_directories(${PROJECT_SOURCE_DIR}/../../include ${PROJECT_SOURCE_DIR})

link_directories("${CMAKE_BINARY_DIR}/tasks")

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} tasks ev)

if(DEBUG EQUAL 1)
    add_definitions(-DDEBUG -DLOGMUTEX)
endif(DEBUG EQUAL 1)

if(PROFILER EQUAL 1)
    add_definitions(-DPROFILER)
    target_link_libraries(${PROJECT_NAME} profiler)
endif(PROFILER EQUAL 1)
