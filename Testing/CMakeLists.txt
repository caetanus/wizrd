cmake_minimum_required(VERSION 2.6)
project(util_tests)

################################
# Unit Tests
################################
# Add test cpp file

macro (make_test)
    foreach(test_name ${ARGN})
        add_executable(${test_name} "${test_name}.cpp")
        target_link_libraries(${test_name} wizrd_ws wizrd_util gtest gtest_main pthread
            ${Boost_LOG_LIBRARY})
        add_test(${test_name} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${test_name})
    endforeach(test_name)

endmacro (make_test)

#add_executable(url_tests url_tests.cpp)
#add_executable(base64_tests base64_test.cpp)
#add_executable(request_handler_tests request_handler_test.cpp)
## Link test executable against gtest & gtest_main
#target_link_libraries(url_tests wizrd_ws wizrd_util gtest gtest_main pthread
#                      ${Boost_LOG_LIBRARY})
#target_link_libraries(base64 wizrd_ws wizrd_util gtest gtest_main pthread
#                      ${Boost_LOG_LIBRARY})
#target_link_libraries(base64 wizrd_ws wizrd_util gtest gtest_main pthread
#                      ${Boost_LOG_LIBRARY})
#add_test(url_tests ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests)
#add_test(base64_tests ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests)

make_test(base64_test
          url_test
          request_handler_test)
