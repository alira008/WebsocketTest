cmake_minimum_required(VERSION 3.24)
include(FetchContent)
include(CMakePrintHelpers)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
# Install dlls in the same directory as the executable on Windows
if(WIN32)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
else()
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_FLAGS
    "-std=gnuc++2b -stdlib=libc++ -IC:/msys64/clang64/include/c++/v1")
set(CMAKE_CXX_FLAGS_DEBUG
    "${CMAKE_CXX_FLAGS_DEBUG} -g3 -Wall -Wextra -O1 -Wformat=2")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(websocket_test)

# LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake") Add packages
include(cmake/cpr.cmake)
include(cmake/openssl.cmake)
include(cmake/simdjson.cmake)
include(cmake/nlohmannjson.cmake)
include(cmake/concurrentqueue.cmake)
include(cmake/readerwriterqueue.cmake)
include(cmake/boost.cmake)
include(cmake/fmt.cmake)

file(GLOB_RECURSE LIB_HEADER_FILES "include/*.h" "src/*.h" "include/*.hpp"
     "src/*.hpp")
file(GLOB_RECURSE LIB_SOURCE_FILES "include/*.cpp" "src/*.cpp")

add_executable(websocket_test ${LIB_HEADER_FILES} ${LIB_SOURCE_FILES})

target_link_libraries(
  websocket_test
  PUBLIC cpr::cpr
         simdjson
         OpenSSL::SSL
         fmt::fmt
         OpenSSL::Crypto
         nlohmann_json::nlohmann_json
         concurrentqueue
         readerwriterqueue
         Boost::headers)

     target_precompile_headers(websocket_test PRIVATE src/pch.hpp)
