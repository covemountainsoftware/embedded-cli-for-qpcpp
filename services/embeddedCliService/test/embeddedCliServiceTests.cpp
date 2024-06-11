/// @brief  Tests for the Embedded-CLI Service
/// @ingroup
/// @cond
///***************************************************************************
///
/// License is noted in the service's LICENSE.txt file (MIT)
///
/// Contact Information:
///   Matthew Eshleman
///   https://covemountainsoftware.com
///   info@covemountainsoftware.com
///***************************************************************************
/// @endcond

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

TEST_GROUP(EmbeddedCliServiceTests)
{
    EmbeddedCLI::Service* mUnderTest = nullptr;
    test::PublishedEventRecorder* mRecorder = nullptr;
    cms::mocks::MockCharacterDevice* mMockCharacterDevice = nullptr;

    void setup() final
    {
        using namespace cms::test;

        qf_ctrl::Setup(MAX_PUB_SUB_SIG, bsp::TICKS_PER_SECOND);
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

    void startService(uint64_t* buffer = nullptr, size_t bufferElementCount = 0, const char * customInvitation = nullptr)
    {
        using namespace cms::test;
        mUnderTest = new EmbeddedCLI::Service(buffer, bufferElementCount, customInvitation);

        mUnderTest->start(qf_ctrl::UNIT_UNDER_TEST_PRIORITY,
                          testQueueStorage.data(), testQueueStorage.size(),
                          nullptr, 0U);
        qf_ctrl::ProcessEvents();
        mock().checkExpectations();
        CHECK_TRUE(mRecorder->isSignalRecorded(EMBEDDED_CLI_INACTIVE_SIG));
    }

    void startServiceToActive(const char * customInvitation = nullptr)
    {
        using namespace cms::test;

        startService(nullptr, 0, customInvitation);
        mock().ignoreOtherCalls();
        mUnderTest->BeginCliAsync(mMockCharacterDevice);
        qf_ctrl::ProcessEvents();
        mock().checkExpectations();
        CHECK_TRUE(mRecorder->isSignalRecorded(EMBEDDED_CLI_ACTIVE_SIG));
    }

    static void mockExpectWritesToCharacterDevice(const Bytes& expectedWrites)
    {
        for (uint8_t byte : expectedWrites)
        {
            mock("CharacterDevice").expectOneCall("WriteAsync").withParameter("byte", byte);
        }
    }
};

TEST(EmbeddedCliServiceTests, when_created_and_initialized_does_not_crash)
{
    //This simple test creates the unit under test
    //and performs the initial start of the service (active object).
    //If this happens without any asserts or crashes, then
    //the test is passing.
    startService();
}


TEST(EmbeddedCliServiceTests, after_initial_init_emits_inactive_signal)
{
    //I like a service to perform minimal initialization
    //and instead wait for some signal to begin its
    //real ongoing work. This allows some higher level
    //application logic to precisely control the
    //system's overall startup order.

    //For convenience during testing, I'm going to have the AO
    //publish an inactive event, which will be useful during
    //testing and would likely be useful to users of this service
    //too.
    startService();

    //NOTE: moved the published event recorder check into startService()
    //      so that the INACTIVE event is always cleared from the
    //      recorder for subsequent tests/event recording.
}

TEST(EmbeddedCliServiceTests, writes_no_data_to_char_device_during_startup)
{
    //I like a service to perform minimal initialization
    //and instead wait for some signal to begin its
    //real ongoing work. This allows some higher level
    //application logic to precisely control the
    //system's overall startup order.

    //Just looking at the embedded-cli, looks like
    //once it is running it will print a greetings
    //message. So, I'll test this by ensuring
    //no bytes are written to the UART yet.

    mock("CharacterDevice").expectNoCall("WriteAsync");
    startService();
}

TEST(EmbeddedCliServiceTests, writes_default_prompt_after_default_activation)
{
    using namespace cms::test;
    startService();

    //We expect the default output of a new CLI to be: "> "
    Bytes expectedWrites = { '>', ' ' };
    mockExpectWritesToCharacterDevice(expectedWrites);
    mock().ignoreOtherCalls();

    mUnderTest->BeginCliAsync(mMockCharacterDevice);
    qf_ctrl::ProcessEvents();
    mock().checkExpectations();
}

TEST(EmbeddedCliServiceTests, publishes_an_event_upon_active)
{
    using namespace cms::test;
    startService();

    //only interested in the pub sub event in this test
    mock().ignoreOtherCalls();

    mUnderTest->BeginCliAsync(mMockCharacterDevice);
    qf_ctrl::ProcessEvents();
    mock().checkExpectations();
    CHECK_TRUE(mRecorder->isSignalRecorded(EMBEDDED_CLI_ACTIVE_SIG));
}

TEST(EmbeddedCliServiceTests, upon_receiving_an_empty_linefeed_will_echo_same_and_prompt)
{
    using namespace cms::test;
    startServiceToActive();

    Bytes expectedWrites = {'\r', '\n', '>', ' ', 0x1b, '[', 's', 0x1b, '[', 'u'};
    mockExpectWritesToCharacterDevice(expectedWrites);
    mock().ignoreOtherCalls();

    //inject characters into our mock character device
    mMockCharacterDevice->InjectCharacterSequence("\n");
    qf_ctrl::ProcessEvents();
    mock().checkExpectations();
}

TEST(EmbeddedCliServiceTests, service_supports_static_memory_for_the_cli)
{
    // I could not find a way to prove via this unit test
    // that the static memory is used and malloc is not used.
    // In theory, I could hijack malloc but the test environment
    // itself is ultimately allocating memory too.
    // Ultimately this particular test was proven manually and this
    // actual test only exercises the exact API which was inspected
    // for correctness.
    using namespace cms::test;
    std::array<uint64_t, 100> staticMemory = {0};

    startService(staticMemory.data(), staticMemory.size());
    mock().ignoreOtherCalls();
    mUnderTest->BeginCliAsync(mMockCharacterDevice);
    qf_ctrl::ProcessEvents();
    mock().checkExpectations();
    CHECK_TRUE(mRecorder->isSignalRecorded(EMBEDDED_CLI_ACTIVE_SIG));
}

TEST(EmbeddedCliServiceTests, service_supports_a_custom_invitation)
{
    using namespace cms::test;
    startServiceToActive("test> ");

    Bytes expectedWrites = {'\r', '\n', 't', 'e', 's', 't', '>', ' ', 0x1b, '[', 's', 0x1b, '[', 'u'};
    mockExpectWritesToCharacterDevice(expectedWrites);
    mock().ignoreOtherCalls();

    //inject characters into our mock character device
    mMockCharacterDevice->InjectCharacterSequence("\n");
    qf_ctrl::ProcessEvents();
    mock().checkExpectations();
}

static void onTestCmd(EmbeddedCli* cli, char* args, void* context)
{
    (void)cli;
    (void)args;
    (void)context;
    mock("TEST").actualCall(__FUNCTION__)
      .withParameter("cli", cli)
      .withParameter("args", args)
      .withParameter("context", context);
}

TEST(EmbeddedCliServiceTests, service_asserts_if_add_cli_binding_when_inactive)
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
    mock().checkExpectations();
}

TEST(EmbeddedCliServiceTests, service_asserts_if_add_cli_binding_cmd_is_null)
{
    using namespace cms::test;
    startServiceToActive();

    MockExpectQAssert();
    mUnderTest->AddCliBindingAsync({
      "testCmd",
      "Help Me!",
      true,
      mUnderTest,
      nullptr
    });
    mock().checkExpectations();
}

TEST(EmbeddedCliServiceTests, service_asserts_if_add_cli_binding_cmd_string_is_null)
{
    using namespace cms::test;
    startServiceToActive();

    MockExpectQAssert();
    mUnderTest->AddCliBindingAsync({
      nullptr,
      "Help Me!",
      true,
      mUnderTest,
      onTestCmd
    });
    mock().checkExpectations();
}