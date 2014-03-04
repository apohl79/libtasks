project(thrift_server)
cmake_minimum_required(VERSION 2.6)

set(DEBUG 0)
set(PROFILER 0)

#add_compile_options(-O4 -ggdb -std=c++11 -pthread)
add_definitions(-O4 -ggdb -std=c++11 -pthread)

include_directories(${PROJECT_SOURCE_DIR}/../../include ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR}/gen-cpp)

execute_process(COMMAND thrift --gen cpp -o ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/test.thrift)
execute_process(COMMAND rm -f ${PROJECT_BINARY_DIR}/gen-cpp/test_service_server.skeleton.cpp)

aux_source_directory(. SOURCES)
aux_source_directory(${PROJECT_BINARY_DIR}/gen-cpp SOURCES)

link_directories("${CMAKE_BINARY_DIR}/tasks")

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} tasks ev thrift)

if(DEBUG EQUAL 1)
    add_definitions(-DDEBUG -DLOGMUTEX)
endif(DEBUG EQUAL 1)

if(PROFILER EQUAL 1)
    add_definitions(-DPROFILER)
    target_link_libraries(${PROJECT_NAME} profiler)
endif(PROFILER EQUAL 1)
