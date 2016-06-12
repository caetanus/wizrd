cmake_minimum_required(VERSION 2.6)
project(url_test)

################################
# Unit Tests
################################
# Add test cpp file


add_executable(url_tests url_tests.cpp )
# Link test executable against gtest & gtest_main
target_link_libraries(url_tests wizrd_ws wizrd_util gtest gtest_main pthread
                      ${Boost_LOG_LIBRARY})
add_test(url_test ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/url_tests)
