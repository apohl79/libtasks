project(libtasks-tests)
cmake_minimum_required(VERSION 2.6)

execute_process(COMMAND thrift --gen cpp -o ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/ipservice.thrift)
execute_process(COMMAND rm -f ${PROJECT_BINARY_DIR}/gen-cpp/IpService_server.skeleton.cpp)

aux_source_directory(. SOURCES)
aux_source_directory(${PROJECT_BINARY_DIR}/gen-cpp SOURCES)

add_definitions(-O4 -Wall -ggdb -std=c++11 -pthread)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
  add_definitions(-D_DEBUG_OUTPUT -DLOGMUTEX)
endif(CMAKE_BUILD_TYPE MATCHES "Debug")

include_directories(${PROJECT_SOURCE_DIR}/../include ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR}/gen-cpp)

link_directories("${CMAKE_BINARY_DIR}/tasks")

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} tasks ev cppunit thrift)
