project(libtasks-root)
cmake_minimum_required(VERSION 2.6)

add_subdirectory(tasks)
add_subdirectory(examples)
add_subdirectory(tests)

file(GLOB tasks_headers "include/tasks/*.h")
file(GLOB net_headers "include/tasks/net/*.h")
file(GLOB tools_headers "include/tasks/tools/*.h")
install(FILES ${tasks_headers} DESTINATION include/tasks)
install(FILES ${net_headers} DESTINATION include/tasks/net)
install(FILES ${tools_headers} DESTINATION include/tasks/tools)

enable_testing()
add_test(NAME UnitTests
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tests 
         COMMAND ${PROJECT_SOURCE_DIR}/tests/runtests.sh ./libtasks-tests ${PROJECT_SOURCE_DIR}/tests)
