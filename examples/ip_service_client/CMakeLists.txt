project(ip_service_client)
cmake_minimum_required(VERSION 2.6)
include(CMakeBase)

find_package(PkgConfig)
find_program(THRIFT_EXECUTABLE NAMES thrift REQUIRED)
pkg_check_modules(THRIFT thrift REQUIRED)

execute_process(COMMAND thrift --gen cpp -o ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/ipservice.thrift)
execute_process(COMMAND rm -f ${PROJECT_BINARY_DIR}/gen-cpp/IpService_server.skeleton.cpp)

include_directories(${PROJECT_SOURCE_DIR}/../../include ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR}/gen-cpp)

aux_source_directory(. SOURCES)
aux_source_directory(${PROJECT_BINARY_DIR}/gen-cpp SOURCES)

link_directories("${CMAKE_BINARY_DIR}/tasks")

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} tasks ev thrift)
