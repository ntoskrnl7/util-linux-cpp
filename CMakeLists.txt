cmake_minimum_required(VERSION 3.10)

project(util-linux-cpp-test)

file(COPY ./deps/nlohmann-json/single_include/nlohmann/json.hpp DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/include/util_linux/nlohmann)
file(COPY ./deps/ext/include/ext DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/include/util_linux)

# 빌드 종류가 명시되지 않았다면 디버그 빌드를 수행하도록 합니다.
if (CMAKE_BUILD_TYPE STREQUAL "")
  set(CMAKE_BUILD_TYPE Debug)
  message("CMAKE_BUILD_TYPE (default) : " ${CMAKE_BUILD_TYPE})
else()
  message("CMAKE_BUILD_TYPE : " ${CMAKE_BUILD_TYPE})
endif()

# gtest를 다운로드 받아서 빌드합니다.
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

include_directories("${gtest_SOURCE_DIR}/include")

# util-linux-cpp-test 테스트 프로그램을 설정합니다
file(GLOB SOURCE_FILES ./tests/*.cpp)
add_executable(util-linux-cpp-test ${SOURCE_FILES})
target_link_libraries(util-linux-cpp-test ${LSF_LIBRARIES} pthread stdc++fs gtest gtest_main)
target_include_directories(util-linux-cpp-test PRIVATE ./include)

# util-linux-cpp-test는 std:any 등을 사용하기 때문에 C++17이후에서만 빌드가 가능하므로
# CXX_STANDARD가 설정 되어있지 않았다면 C++17로 설정합니다.
get_property(CXX_STANDARD_SPECIFIED TARGET util-linux-cpp-test PROPERTY CXX_STANDARD SET)
if (CXX_STANDARD_SPECIFIED)
  get_property(CXX_STANDARD_VAR TARGET util-linux-cpp-test PROPERTY CXX_STANDARD)
  message("CXX_STANDARD : " ${CXX_STANDARD_VAR})
else()
  set_property(TARGET util-linux-cpp-test PROPERTY CXX_STANDARD 17)
  get_property(CXX_STANDARD_VAR TARGET util-linux-cpp-test PROPERTY CXX_STANDARD)
  message("CXX_STANDARD (default): " ${CXX_STANDARD_VAR})
endif()
set_property(TARGET util-linux-cpp-test PROPERTY CXX_STANDARD_REQUIRED ON)

enable_testing()
add_test(NAME util-linux-cpp-test COMMAND util-linux-cpp-test)