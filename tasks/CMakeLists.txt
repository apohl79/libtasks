project(tasks)
cmake_minimum_required(VERSION 2.6)
get_property(LIB64 GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS)

find_package(PkgConfig REQUIRED)
find_package(Boost 1.53.0 REQUIRED system thread REQUIRED)
find_package(LibEV REQUIRED)

if(LIB64 AND (CMAKE_SYSTEM_NAME STREQUAL Linux))
  set(LIB_SUFFIX 64)
else(LIB64 AND (CMAKE_SYSTEM_NAME STREQUAL Linux))
  set(LIB_SUFFIX "")
endif(LIB64 AND (CMAKE_SYSTEM_NAME STREQUAL Linux))

set(INSTALL_LIB_DIR lib${LIB_SUFFIX} CACHE PATH "Installation directory for libraries")
mark_as_advanced(INSTALL_LIB_DIR)
file(STRINGS ${PROJECT_SOURCE_DIR}/../VERSION LIBRARY_VERSION)

configure_file(
  ${PROJECT_SOURCE_DIR}/../pkgconfig/libtasks.pc.in
  ${PROJECT_BINARY_DIR}/pkgconfig/libtasks.pc
  @ONLY)

aux_source_directory(. TASKS_SRC)
aux_source_directory(tools TASKS_SRC)
aux_source_directory(net TASKS_SRC)
include_directories(${PROJECT_SOURCE_DIR}/../include)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${LIBEV_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
link_directories(${LIBEV_LIBRARY_DIRS})


add_definitions(-Wall -Wextra -std=c++11 -pthread)
add_definitions(-DTASKS_VERSION="${LIBRARY_VERSION}")

if(CMAKE_SYSTEM_NAME STREQUAL Linux)
  add_definitions(-O4 -D_OS_LINUX_)
  execute_process(COMMAND getconf LEVEL1_DCACHE_LINESIZE OUTPUT_VARIABLE CACHE_LINE_SIZE OUTPUT_STRIP_TRAILING_WHITESPACE)
elseif(CMAKE_SYSTEM_NAME STREQUAL Darwin)
  add_definitions(-O3 -D_OS_DARWIN_)
  execute_process(COMMAND sysctl -n hw.cachelinesize OUTPUT_VARIABLE CACHE_LINE_SIZE OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

add_definitions(-DCACHE_LINE_SIZE=${CACHE_LINE_SIZE})

if(CMAKE_BUILD_TYPE MATCHES "Debug")
  add_definitions(-D_DEBUG_OUTPUT -DLOGMUTEX)
  if(CMAKE_SYSTEM_NAME STREQUAL Linux)
    add_definitions(-ggdb)
  elseif(CMAKE_SYSTEM_NAME STREQUAL Darwin)
    add_definitions(-g)
  endif()
endif(CMAKE_BUILD_TYPE MATCHES "Debug")

add_library(${PROJECT_NAME} SHARED ${TASKS_SRC})
target_link_libraries(${PROJECT_NAME} -lpthread ${LIBEV_LIBRARIES})
set_target_properties(${PROJECT_NAME} PROPERTIES
  VERSION ${LIBRARY_VERSION}
  SOVERSION ${LIBRARY_VERSION}
  )
install(TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_LIB_DIR})
install(FILES ${PROJECT_BINARY_DIR}/pkgconfig/libtasks.pc DESTINATION ${INSTALL_LIB_DIR}/pkgconfig)
