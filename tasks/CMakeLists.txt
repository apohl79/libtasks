project(tasks)
cmake_minimum_required(VERSION 2.6)

aux_source_directory(. TASKS_SRC)
aux_source_directory(tools TASKS_SRC)
aux_source_directory(net TASKS_SRC)
include_directories(${PROJECT_SOURCE_DIR}/../include)

file(STRINGS ${PROJECT_SOURCE_DIR}/../VERSION LIBRARY_VERSION)
#add_compile_options(-O4 -ggdb -std=c++11 -pthread)
add_definitions(-O4 -Wall -ggdb -std=c++11 -pthread)
add_definitions(-DTASKS_VERSION="${LIBRARY_VERSION}")

if(CMAKE_SYSTEM_NAME STREQUAL Linux)
  execute_process(COMMAND getconf LEVEL1_DCACHE_LINESIZE OUTPUT_VARIABLE CACHE_LINE_SIZE OUTPUT_STRIP_TRAILING_WHITESPACE)
elseif(CMAKE_SYSTEM_NAME STREQUAL Darwin)
  execute_process(COMMAND sysctl -n hw.cachelinesize OUTPUT_VARIABLE CACHE_LINE_SIZE OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

add_definitions(-DCACHE_LINE_SIZE=${CACHE_LINE_SIZE})

if(CMAKE_BUILD_TYPE MATCHES "Debug")
  add_definitions(-D_DEBUG_OUTPUT -DLOGMUTEX)
endif(CMAKE_BUILD_TYPE MATCHES "Debug")

add_library(${PROJECT_NAME} SHARED ${TASKS_SRC})
target_link_libraries(${PROJECT_NAME} -lpthread ev)

install(TARGETS ${PROJECT_NAME} DESTINATION lib64)
