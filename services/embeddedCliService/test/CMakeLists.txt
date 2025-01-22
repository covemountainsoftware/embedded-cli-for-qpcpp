# prep for cpputest based build
set(TEST_APP_NAME EmbeddedCliServiceTests)

include_directories(${CMS_CHAR_DEVICE_INCLUDE})
include_directories(${CMS_MOCK_CHAR_DEVICE_DIR})
include_directories(../include)

set(TEST_SOURCES
        embeddedCliServiceTests.cpp
        embeddedCliServiceTestsWithoutPoolLeakDetection.cpp
        ../src/embeddedCliService.cpp
        ../src/embedded_cli_impl.c
        ${CMS_MOCK_CHAR_DEVICE_DIR}/mockCharacterDevice.cpp
)

# this include expects TEST_SOURCES and TEST_APP_NAME to be
# defined, and creates the cpputest based test executable target
include(${CMS_CMAKE_DIR}/cpputestCMake.cmake)

# Add in the CppUTest malloc tie-in for malloc memory leak detection
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -include CppUTest/MemoryLeakDetectorMallocMacros.h")

target_link_libraries(${TEST_APP_NAME} cpputest-for-qpcpp-lib ${CPPUTEST_LDFLAGS})