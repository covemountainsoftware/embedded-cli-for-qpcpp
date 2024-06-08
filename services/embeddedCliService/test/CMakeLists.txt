# prep for cpputest based build
set(TEST_APP_NAME EmbeddedCliServiceTests)

set(TEST_SOURCES
        embeddedCliServiceTests.cpp
        ../src/embeddedCliService.cpp)

# this include expects TEST_SOURCES and TEST_APP_NAME to be
# defined, and creates the cpputest based test executable target
include(${CMS_CMAKE_DIR}/cpputestCMake.cmake)

target_link_libraries(${TEST_APP_NAME} cpputest-for-qpcpp-lib ${CPPUTEST_LDFLAGS})