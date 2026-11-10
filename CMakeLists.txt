cmake_minimum_required(VERSION 3.24)
project(CppLessons LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/tests/*_test.cpp")
foreach(test_src ${TEST_SOURCES})
  get_filename_component(test_name ${test_src} NAME_WE)
  add_executable(${test_name} ${test_src})
  target_include_directories(${test_name} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lessons")
  target_compile_options(${test_name} PRIVATE -Wall -Wextra -Wpedantic -Werror -O2)
  add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
