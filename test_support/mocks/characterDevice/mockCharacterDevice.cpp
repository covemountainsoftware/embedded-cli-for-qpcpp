
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

void MockCharacterDevice::RegisterNewByteCallback(void* userData)
{
    (void)userData;
}

} //namespace mocks
} //namespace cms