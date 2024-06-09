#include <cstring>
#include "mockCharacterDevice.hpp"
#include "CppUTestExt/MockSupport.h"

static constexpr const char* MOCK_NAME = "CharacterDevice";

namespace cms {
namespace mocks {

bool MockCharacterDevice::WriteAsync(uint8_t byte)
{
    mock(MOCK_NAME).actualCall("WriteAsync").withParameter("byte", byte);
    return static_cast<bool>(mock(MOCK_NAME).returnIntValueOrDefault(true)); //use IntValue due to bug in CppUTest 3.8 bool handling.
}

void MockCharacterDevice::RegisterNewByteCallback(NewByteCallback callback, void* userData)
{
    mCallback = callback;
    mUserData = userData;
}

void MockCharacterDevice::InjectCharacterSequence(const char* inject)
{
    if (mCallback == nullptr)
    {
        //noting to do, just return
        return;
    }

    size_t injectLength = strlen(inject);
    for (size_t i = 0; i < injectLength; ++i)
    {
        mCallback(mUserData, static_cast<uint8_t>(inject[i]));
    }
}

} //namespace mocks
} //namespace cms