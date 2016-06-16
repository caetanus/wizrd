cmake_minimum_required(VERSION 2.6)
project(util_tests)

################################
# Unit Tests
################################
# Add test cpp file

aux_source_directory(. FILES)
#add_executable(url_tests url_tests.cpp)
#add_executable(base64_tests base64_test.cpp)
add_executable(tests ${FILES})
# Link test executable against gtest & gtest_main
# target_link_libraries(url_tests wizrd_ws wizrd_util gtest gtest_main pthread
#                      ${Boost_LOG_LIBRARY})
target_link_libraries(tests wizrd_ws wizrd_util gtest gtest_main pthread
                      ${Boost_LOG_LIBRARY})
#add_test(url_tests ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests)
#add_test(base64_tests ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests)
add_test(tests ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests)
