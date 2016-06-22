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

make_test(base64_test
          url_test
          request_handler_test)
