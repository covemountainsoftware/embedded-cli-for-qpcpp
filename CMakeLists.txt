cmake_minimum_required(VERSION 3.16)
project(cpputest-for-qpcpp-examples)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 11)

add_compile_options(-Wall -Wextra -Werror)

include_directories(include)

set(EXTERNALS_TOP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/externals)

# set CMS_QPCPP_TOP_DIR to avoid cpputest-for-qpcpp-lib from fetching its own copy of QP/C++
set(CMS_QPCPP_TOP_DIR ${EXTERNALS_TOP_DIR}/qpcpp)

add_subdirectory(externals/cpputest-for-qpcpp)

set(CMS_TEST_SUPPORT_TOP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test_support)
set(MOCKS_TOP_DIR ${CMS_TEST_SUPPORT_TOP_DIR}/mocks)
set(DRIVERS_TOP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/drivers)

set(QP_CPP_INCLUDE_DIR ${CMS_QPC_TOP_DIR}/include)

add_subdirectory(drivers)
add_subdirectory(examples)
