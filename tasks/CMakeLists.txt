project(tasks)
cmake_minimum_required(VERSION 2.6)
include(CMakeBase)

get_property(LIB64 GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS)
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

add_definitions(-DTASKS_VERSION="${LIBRARY_VERSION}")

add_library(${PROJECT_NAME} SHARED ${TASKS_SRC})
target_link_libraries(${PROJECT_NAME} -lpthread ${LIBEV_LIBRARIES})
set_target_properties(${PROJECT_NAME} PROPERTIES
  VERSION ${LIBRARY_VERSION}
  SOVERSION ${LIBRARY_VERSION}
  )
install(TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_LIB_DIR})
install(FILES ${PROJECT_BINARY_DIR}/pkgconfig/libtasks.pc DESTINATION ${INSTALL_LIB_DIR}/pkgconfig)
