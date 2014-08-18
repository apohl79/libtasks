project(libtasks-tests)
cmake_minimum_required(VERSION 2.6)

find_package(PkgConfig)
find_package(Boost 1.53.0 COMPONENTS system REQUIRED)
find_program(THRIFT_EXECUTABLE NAMES thrift REQUIRED)
pkg_check_modules(THRIFT thrift REQUIRED)
pkg_check_modules(EV libev REQUIRED)
pkg_check_modules(CPPUNIT cppunit)

execute_process(COMMAND thrift --gen cpp -o ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/ipservice.thrift)
execute_process(COMMAND rm -f ${PROJECT_BINARY_DIR}/gen-cpp/IpService_server.skeleton.cpp)

add_definitions(-Wall -Wextra -std=c++11 -pthread)

if(CMAKE_SYSTEM_NAME STREQUAL Linux)
  add_definitions(-O4 -D_OS_LINUX_)
elseif(CMAKE_SYSTEM_NAME STREQUAL Darwin)
  add_definitions(-O3 -D_OS_DARWIN_)
endif()


if(CMAKE_BUILD_TYPE MATCHES "Debug")
  add_definitions(-D_DEBUG_OUTPUT -DLOGMUTEX)
  if(CMAKE_SYSTEM_NAME STREQUAL Linux)
    add_definitions(-ggdb)
  elseif(CMAKE_SYSTEM_NAME STREQUAL Darwin)
    add_definitions(-g)
  endif()
endif(CMAKE_BUILD_TYPE MATCHES "Debug")

include_directories(${PROJECT_SOURCE_DIR}/../include)
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_BINARY_DIR}/gen-cpp)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${EV_INCLUDE_DIRS})
include_directories(${THRIFT_INCLUDE_DIRS})

aux_source_directory(. SOURCES)
aux_source_directory(${PROJECT_BINARY_DIR}/gen-cpp SOURCES)

link_directories("${CMAKE_BINARY_DIR}/tasks")
link_directories(${Boost_LIBRARY_DIRS})
link_directories(${EV_LIBRARY_DIRS})
link_directories(${THRIFT_LIBRARY_DIRS})

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} tasks)
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${EV_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${CPPUNIT_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${THRIFT_LIBRARIES})
