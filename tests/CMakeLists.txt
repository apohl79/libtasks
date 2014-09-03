project(libtasks-tests)
cmake_minimum_required(VERSION 2.6)
include(CMakeBase)

find_package(PkgConfig)
find_program(THRIFT_EXECUTABLE NAMES thrift REQUIRED)
pkg_check_modules(THRIFT thrift REQUIRED)
pkg_check_modules(CPPUNIT cppunit)

execute_process(COMMAND thrift --gen cpp -o ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/ipservice.thrift)
execute_process(COMMAND rm -f ${PROJECT_BINARY_DIR}/gen-cpp/IpService_server.skeleton.cpp)

include_directories(${PROJECT_SOURCE_DIR}/../include)
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_BINARY_DIR}/gen-cpp)
include_directories(${THRIFT_INCLUDE_DIRS})

aux_source_directory(. SOURCES)
aux_source_directory(${PROJECT_BINARY_DIR}/gen-cpp SOURCES)

link_directories("${CMAKE_BINARY_DIR}/tasks")
link_directories(${THRIFT_LIBRARY_DIRS})

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} tasks)
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${LIBEV_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${CPPUNIT_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${THRIFT_LIBRARIES})
