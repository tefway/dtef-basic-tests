cmake_minimum_required(VERSION 3.5)
project(DTEF_CLI CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(CTest)
enable_testing()
find_package(GTest)

if(GTest_FOUND)
    include(GoogleTest)
endif()

find_package(Poco REQUIRED COMPONENTS Foundation Net Util JSON)

file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(ws_server ${SOURCES})
target_link_libraries(ws_server Poco::Foundation Poco::Net Poco::Util Poco::JSON)

if(GTest_FOUND)
    add_subdirectory(${CMAKE_SOURCE_DIR}/tests)
endif()
