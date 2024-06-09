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
#include "cms_cpputest_qf_ctrl.hpp"
#include "cmsTestPublishedEventRecorder.hpp"
#include "pubsub_signals.hpp"
#include "bspTicks.hpp"
#include "mockCharacterDevice.hpp"

// the cpputest headers must always be last
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

using namespace cms;

static std::array<QP::QEvt const*, 10> testQueueStorage;

TEST_GROUP(EmbeddedCliServiceTests)
{
    EmbeddedCLI::Service* mUnderTest = nullptr;
    test::PublishedEventRecorder* mRecorder = nullptr;

    void setup() final
    {
        using namespace cms::test;

        qf_ctrl::Setup(PubSub::MAX_PUB_SIG, bsp::TICKS_PER_SECOND);
        mRecorder = cms::test::PublishedEventRecorder::CreatePublishedEventRecorder(
          qf_ctrl::RECORDER_PRIORITY, QP::Q_USER_SIG, PubSub::MAX_PUB_SIG);
        mUnderTest = new EmbeddedCLI::Service();
    }

    void teardown() final
    {
        using namespace cms::test;

        delete mUnderTest;
        mock().clear();
        qf_ctrl::Teardown();
        delete mRecorder;
    }

    void startService() const
    {
        using namespace cms::test;

        mUnderTest->start(qf_ctrl::UNIT_UNDER_TEST_PRIORITY,
                          testQueueStorage.data(), testQueueStorage.size(),
                          nullptr, 0U);
        qf_ctrl::ProcessEvents();
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
    CHECK_TRUE(mRecorder->isSignalRecorded(PubSub::EMBEDDED_CLI_INACTIVE_SIG));
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
    mock().checkExpectations();
}

TEST(EmbeddedCliServiceTests, writes_data_to_char_device_after_default_activation)
{
    using namespace cms::test;

    startService();
    mock().checkExpectations();

    //the number of bytes written will depend on the behavior of the internal
    //embedded-cli. Therefore, this test is a bit fragile, and we will have to
    //determine how many bytes will be written as an experiment.
    mock("CharacterDevice").expectNCalls(2, "WriteAsync").ignoreOtherParameters();
    mock().ignoreOtherCalls();

    auto myMock = new cms::mocks::MockCharacterDevice();
    mUnderTest->BeginCliAsync(myMock);
    qf_ctrl::ProcessEvents();
    mock().checkExpectations();
    delete myMock;
}
