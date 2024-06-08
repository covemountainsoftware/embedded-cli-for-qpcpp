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
#include "pubsub_signals.hpp"
#include "bspTicks.hpp"

// the cpputest headers must always be last
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

using namespace cms;

static std::array<QP::QEvt const*, 10> testQueueStorage;

TEST_GROUP(EmbeddedCliServiceTests)
{
    EmbeddedCli::Service* mUnderTest = nullptr;

    void setup() final
    {
        using namespace cms::test;

        qf_ctrl::Setup(PubSub::MAX_PUB_SIG, bsp::TICKS_PER_SECOND);

        mUnderTest = new EmbeddedCli::Service();
    }

    void teardown() final
    {
        using namespace cms::test;

        delete mUnderTest;

        mock().clear();
        qf_ctrl::Teardown();
    }

    void startService() const
    {
        using namespace cms::test;

        mUnderTest->start(qf_ctrl::UNIT_UNDER_TEST_PRIORITY,
                          testQueueStorage.data(), testQueueStorage.size(),
                          nullptr, 0U);
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