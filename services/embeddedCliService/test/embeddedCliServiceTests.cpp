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

#include <array>
#include "cms_cpputest_qf_ctrl.hpp"
#include "pubsub_signals.hpp"
#include "bspTicks.hpp"

// the cpputest headers must always be last
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

TEST_GROUP(EmbeddedCliServiceTests)
{
    void setup() final
    {
        using namespace cms::test;

        qf_ctrl::Setup(PubSub::MAX_PUB_SIG, bsp::TICKS_PER_SECOND);
    }

    void teardown() final
    {
        using namespace cms::test;

        mock().clear();
        qf_ctrl::Teardown();
    }
};

TEST(EmbeddedCliServiceTests, when_created_and_initialized_does_not_crash)
{
    //TODO
}