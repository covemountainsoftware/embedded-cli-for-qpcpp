# prep for cpputest based build
set(TEST_APP_NAME EmbeddedCliServiceTests)

include_directories(${CMS_CHAR_DEVICE_INCLUDE})
include_directories(${CMS_MOCK_CHAR_DEVICE_DIR})

include(CMakePrintHelpers)
cmake_print_variables(CMS_MOCK_CHAR_DEVICE_DIR)

set(TEST_SOURCES
        embeddedCliServiceTests.cpp
        ../src/embeddedCliService.cpp
        ${CMS_MOCK_CHAR_DEVICE_DIR}/mockCharacterDevice.cpp
)

# this include expects TEST_SOURCES and TEST_APP_NAME to be
# defined, and creates the cpputest based test executable target
include(${CMS_CMAKE_DIR}/cpputestCMake.cmake)


target_link_libraries(${TEST_APP_NAME} cpputest-for-qpcpp-lib ${CPPUTEST_LDFLAGS})