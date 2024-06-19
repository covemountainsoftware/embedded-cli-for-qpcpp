
#ifndef EMBEDDED_CLI_FOR_QPCPP_LINUXCHARACTERDEVICE_HPP
#define EMBEDDED_CLI_FOR_QPCPP_LINUXCHARACTERDEVICE_HPP

#include "characterDeviceInterface.hpp"
#include <unistd.h>
#include <thread>
#include <cstdlib>
#include <cstdio>

class LinuxCharacterDevice : public cms::interfaces::CharacterDevice
{
public:
    LinuxCharacterDevice() :
        mCallback(nullptr),
        mCallbackUserData(nullptr),
        mReader(&LinuxCharacterDevice::Reader, this)
    {
    }

    bool WriteAsync(uint8_t byte) override
    {
        ssize_t rtn = write(STDOUT_FILENO, &byte, 1);
        return rtn == 1;
    }

    void RegisterNewByteCallback(NewByteCallback callback, void* userData) override
    {
        mCallback = callback;
        mCallbackUserData = userData;
    }

private:
    void Reader()
    {
        char buf;
        ssize_t result = -1;
        do
        {
            result = read(STDIN_FILENO, &buf, 1);
            if (mCallback != nullptr)
            {
                mCallback(mCallbackUserData, buf);
            }
        }  while (result > 0);
    }

    NewByteCallback mCallback = nullptr;
    void* mCallbackUserData = nullptr;
    std::thread mReader = {};
};

#endif   // EMBEDDED_CLI_FOR_QPCPP_LINUXCHARACTERDEVICE_HPP
