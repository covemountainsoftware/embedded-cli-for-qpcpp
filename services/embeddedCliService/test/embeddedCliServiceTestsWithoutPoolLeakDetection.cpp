#include "embeddedCliService.hpp"
#include <array>
#include <vector>
#include "cms_cpputest_qf_ctrl.hpp"
#include "cmsTestPublishedEventRecorder.hpp"
#include "pubsub_signals.hpp"
#include "bspTicks.hpp"
#include "mockCharacterDevice.hpp"

// the cpputest headers must always be last
#include "qassertMockSupport.hpp"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

using namespace cms;

static std::array<QP::QEvt const*, 10> testQueueStorage;

using Bytes = std::vector<uint8_t>;

// A separate test group was needed here, to test internal
// assert test cases, where a dynamic event would 'leak'
// since the assert would interrupt normal event processing.
// This test group disables the cpputest-for-qpcpp memory pool
// leak checks.

TEST_GROUP(EmbeddedCliServiceTestsWithoutMemPoolLeakDetect)
{
    EmbeddedCLI::Service* mUnderTest = nullptr;
    test::PublishedEventRecorder* mRecorder = nullptr;
    cms::mocks::MockCharacterDevice* mMockCharacterDevice = nullptr;

    void setup() final
    {
        using namespace cms::test;
        qf_ctrl::Setup(MAX_PUB_SUB_SIG, bsp::TICKS_PER_SECOND, qf_ctrl::MemPoolConfigs {},
                       qf_ctrl::MemPoolTeardownOption::IGNORE);

        mRecorder = cms::test::PublishedEventRecorder::CreatePublishedEventRecorder(
          qf_ctrl::RECORDER_PRIORITY, QP::Q_USER_SIG, MAX_PUB_SUB_SIG);

        mMockCharacterDevice = new cms::mocks::MockCharacterDevice();
    }

    void teardown() final
    {
        using namespace cms::test;

        delete mUnderTest;
        mock().clear();
        qf_ctrl::Teardown();
        delete mRecorder;
        delete mMockCharacterDevice;
    }

    void startService(uint64_t* buffer = nullptr, size_t bufferElementCount = 0, uint16_t maxBindingCount = 0)
    {
        using namespace cms::test;

        mUnderTest = new EmbeddedCLI::Service(buffer, bufferElementCount, maxBindingCount, nullptr);

        mUnderTest->start(qf_ctrl::UNIT_UNDER_TEST_PRIORITY,
                          testQueueStorage.data(), testQueueStorage.size(),
                          nullptr, 0U);
        qf_ctrl::ProcessEvents();
        mock().checkExpectations();
        CHECK_TRUE(mRecorder->isSignalRecorded(CMS_EMBEDDED_CLI_INACTIVE_SIG));
    }
};

TEST(EmbeddedCliServiceTestsWithoutMemPoolLeakDetect, service_asserts_if_static_memory_is_too_small)
{
    using namespace cms::test;
    std::array<uint64_t, 1> staticMemory = {0}; //too small, will cause an assert

    mock(QASSERT_MOCK_NAME).expectOneCall(ONERROR_FUNC_NAME).ignoreOtherParameters();
    startService(staticMemory.data(), staticMemory.size());
    mock().checkExpectations();
}


static void onTestCmd(EmbeddedCli* cli, char* args, void* context)
{
    (void)cli;
    (void)args;
    (void)context;
}

TEST(EmbeddedCliServiceTestsWithoutMemPoolLeakDetect, service_asserts_if_add_cli_binding_when_inactive)
{
    using namespace cms::test;
    startService();

    MockExpectQAssert();
    mUnderTest->AddCliBindingAsync({
      "testCmd",
      "Help Me!",
      true,
      mUnderTest,
      onTestCmd
    });
    qf_ctrl::ProcessEvents();
    mock().checkExpectations();
}

TEST(EmbeddedCliServiceTestsWithoutMemPoolLeakDetect, the_service_supports_configurable_max_cli_binding_count)
{
    using namespace cms::test;
    startService(nullptr, 0, 1);
    mock().ignoreOtherCalls();

    mUnderTest->BeginCliAsync(mMockCharacterDevice);
    qf_ctrl::ProcessEvents();
    mock().checkExpectations();
    CHECK_TRUE(mRecorder->isSignalRecorded(CMS_EMBEDDED_CLI_ACTIVE_SIG));

    mUnderTest->AddCliBindingAsync({
      "testing",
      "Help Me!",
      true,
      mUnderTest,
      onTestCmd
    });
    qf_ctrl::ProcessEvents();
    mock().checkExpectations();
    mock().clear();

    //adding the second CLI command should assert, as we
    //configured this test instance to support only a single CLI
    //command.
    MockExpectQAssert();
    mUnderTest->AddCliBindingAsync({
      "temp",
      "Temp Me!",
      true,
      mUnderTest,
      onTestCmd
    });
    qf_ctrl::ProcessEvents();
    mock().checkExpectations();
}
