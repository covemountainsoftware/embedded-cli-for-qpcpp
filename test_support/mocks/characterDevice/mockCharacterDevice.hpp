

#ifndef EMBEDDED_CLI_FOR_QPCPP_MOCKCHARACTERDEVICE_HPP
#define EMBEDDED_CLI_FOR_QPCPP_MOCKCHARACTERDEVICE_HPP

#include "characterDeviceInterface.hpp"

namespace cms {
namespace mocks {

class MockCharacterDevice : public cms::interfaces::CharacterDevice
{
public:
    MockCharacterDevice() = default;
    virtual ~MockCharacterDevice() = default;

    bool WriteAsync(uint8_t byte) override;
    void RegisterNewByteCallback(NewByteCallback callback, void* userData) override;

    //unit test specific access
    void InjectCharacterSequence(const char * inject);

private:
    NewByteCallback mCallback = nullptr;
    void* mUserData = nullptr;
};

} //namespace mocks
} //namespace cms

#endif   // EMBEDDED_CLI_FOR_QPCPP_MOCKCHARACTERDEVICE_HPP
