# Copyright (C) 2015 Kenneth Benzie
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
# OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
# OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

cmake_minimum_required(VERSION 2.8)
project(JSON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics")
  endif()
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(JSON
  ${CMAKE_CURRENT_SOURCE_DIR}/include/json/json.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/json.cpp)

option(JSON_BUILD_TESTS "Enable building of JSON tests." OFF)
if(${JSON_BUILD_TESTS})
  add_executable(UnitJSON
    ${CMAKE_CURRENT_SOURCE_DIR}/test/main.cpp)
  target_link_libraries(UnitJSON JSON)
endif()

option(JSON_BUILD_TOOLS "Enable building of JSON tools." OFF)
if(${JSON_BUILD_TOOLS})
  add_executable(jsonv
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/jsonv.cpp)
  target_link_libraries(jsonv JSON)
endif()
