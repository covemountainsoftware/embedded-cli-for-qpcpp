
#ifndef EMBEDDED_CLI_FOR_QPCPP_CHARACTERDEVICEINTERFACE_HPP
#define EMBEDDED_CLI_FOR_QPCPP_CHARACTERDEVICEINTERFACE_HPP

#include <cstdint>

namespace cms {
namespace interfaces {

/**
 * Pure virtual interface for a simply 100% asynchronous
 * character device.
 */
class CharacterDevice {
public:
    typedef void (*NewByteCallback)(void* userData, uint8_t byte);

    /**
     * Write a single byte to the output of this device.
     * Asynchronous.
     * @param byte
     * @return true - write began as expected.
     *         false - some error.
     */
    virtual bool WriteAsync(uint8_t byte) = 0;

    /**
     * Register a callback to be executed on each new
     * incoming byte received on this device.
     *
     * Users should assume this callback is likely executed
     * from within a separate thread or ISR context.
     *
     * @param callback - function ptr of a callback function
     *                   to be executed for each new byte
     *                   received.
     *
     * @param userData - ptr to something of interest to the
     *                   user of this object. Typically a 'this'
     *                   pointer.
     */
    virtual void RegisterNewByteCallback(NewByteCallback callback, void* userData) = 0;
};

} // namespace interfaces
} // namespace cms

#endif   // EMBEDDED_CLI_FOR_QPCPP_CHARACTERDEVICEINTERFACE_HPP
