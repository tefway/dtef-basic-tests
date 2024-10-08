
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

set(srcdir "${CMAKE_CURRENT_SOURCE_DIR}")
set(msrcdir "${CMAKE_CURRENT_SOURCE_DIR}/../src")
set(mainbinary "${CMAKE_CURRENT_SOURCE_DIR}/../bin")

file(
    GLOB ALL_TESTS_SOURCES 
    "${srcdir}/*.cpp"
    "${msrcdir}/ClasseIntegracao.cpp"
)

add_executable(maintest_binary  ${ALL_TESTS_SOURCES})

set(TEST_COMPILE_FLAGS -fsanitize=address -fsanitize=return)
set(TEST_LINK_FLAGS -fsanitize=address -fsanitize=return)

#target_precompile_headers(maintest_binary PRIVATE "${msrcdir}/stdafx.hpp")

target_compile_options(maintest_binary PUBLIC ${TEST_COMPILE_FLAGS})
target_link_options(maintest_binary PUBLIC ${TEST_LINK_FLAGS})
target_compile_definitions(maintest_binary PUBLIC MAIN_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
target_link_libraries(maintest_binary GTest::GTest GTest::Main)

if(Poco_FOUND)
    message("-- Poco encontrado!")
    target_link_libraries(maintest_binary ${Poco_LIBRARIES})
else()
    message(FATAL_ERROR "-- Poco is not found in your system.")
endif(Poco_FOUND)

gtest_discover_tests(maintest_binary)

