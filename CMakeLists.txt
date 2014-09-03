project(libtasks-root)
cmake_minimum_required(VERSION 2.6)

if(NOT DEFINED CMAKE_MACOSX_RPATH)
  set(CMAKE_MACOSX_RPATH 0)
endif()

file(GLOB tasks_headers "include/tasks/*.h")
file(GLOB net_headers "include/tasks/net/*.h")
file(GLOB tools_headers "include/tasks/tools/*.h")
install(FILES ${tasks_headers} DESTINATION include/tasks)
install(FILES ${net_headers} DESTINATION include/tasks/net)
install(FILES ${tools_headers} DESTINATION include/tasks/tools)

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

add_subdirectory(tasks)

if(WITH_EXAMPLES MATCHES "y" OR WITH_EXAMPLES MATCHES "Y")
  add_subdirectory(examples)
endif(WITH_EXAMPLES MATCHES "y" OR WITH_EXAMPLES MATCHES "Y")

if(NOT(DISABLE_TESTS MATCHES "y" OR DISABLE_TESTS MATCHES "Y"))
  add_subdirectory(tests)
  enable_testing()
  add_test(NAME UnitTests_single_loop
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tests
    COMMAND bash ${PROJECT_SOURCE_DIR}/tests/runtests.sh ./libtasks-tests ${PROJECT_SOURCE_DIR}/tests single)
  add_test(NAME UnitTests_multi_loop
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tests
    COMMAND bash ${PROJECT_SOURCE_DIR}/tests/runtests.sh ./libtasks-tests ${PROJECT_SOURCE_DIR}/tests multi)
endif(NOT(DISABLE_TESTS MATCHES "y" OR DISABLE_TESTS MATCHES "Y"))
