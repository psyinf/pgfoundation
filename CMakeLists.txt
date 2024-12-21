cmake_minimum_required(VERSION 3.22)
# detect if being bundled,
# disable testsuite in that case

option(CODE_COVERAGE "Enable coverage reporting" OFF)
if(NOT DEFINED PROJECT_NAME)
  set(NOT_SUBPROJECT ON)
else()
  set(NOT_SUBPROJECT OFF)
endif()


project(pgfoundation)
set(CMAKE_CXX_STANDARD 23)  
set(CMAKE_CXX_STANDARD_REQUIRED True)


include(cmake/PreventInSourceBuilds.cmake)
include(cmake/configure_analysis.cmake)
include(cmake/stacktrace.cmake)
include(cmake/setup_cpm.cmake)
include(cmake/deps_cpm.cmake)

add_subdirectory(pgfoundation)
if (NOT_SUBPROJECT)
  add_subdirectory(examples)
  add_subdirectory(test)
endif()


