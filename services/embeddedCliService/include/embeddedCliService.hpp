/// @brief  The Embedded-CLI Service
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

#ifndef CMS_EMBEDDED_CLI_SERVICE_HPP
#define CMS_EMBEDDED_CLI_SERVICE_HPP

#include <cstdint>
#include <cstddef>
#include <array>
#include "qpcpp.hpp"
#include "pubsub_signals.hpp"
#include "characterDeviceInterface.hpp"
#include "embeddedCliCommandBinding.hpp"
#include "embeddedCliEvent.hpp"
#include "cms_embedded_cli_signal_range.hpp"

//forward declare the third-party EmbeddedCli structs
struct EmbeddedCli;
struct EmbeddedCliConfig;

namespace cms {
namespace EmbeddedCLI { //note, all caps CLI needed to avoid conflicts

// used for proper alignment of cli buffer, below
// matches with embedded cli itself
#if UINTPTR_MAX == 0xFFFF
using CliUint = uint16_t;
#elif UINTPTR_MAX == 0xFFFFFFFF
using CliUint = uint32_t;
#elif UINTPTR_MAX == 0xFFFFFFFFFFFFFFFFu
using CliUint = uint64_t;
#else
    #error unsupported pointer size
#endif


/**
 * The EmbeddedCLI::Service creates a command line interface
 * using a provided character device.
 *
 * Internally uses the third party library 'embedded-cli'
 * available at: https://github.com/funbiscuit/embedded-cli
 */
class Service final : public QP::QActive {
public:
    /**
     * Constructor
     * @param buffer - set to nullptr and the internal CLI will malloc
     *                 the necessary buffer
     * @param bufferElementCount - the size of the provided buffer
     * @param maxBindingCount - the maximum number of CLI commands that
     *                          can be added to this CLI. Set to zero to
     *                          use internal default value.
     * @param customInvitation - a custom string for the CLI prompt.
     *                           Set to nullptr for the internal default prompt
     */
    explicit Service(CliUint* buffer, size_t bufferElementCount, uint16_t maxBindingCount, const char * customInvitation = nullptr);
    ~Service();

    Service(const Service&)            = delete;
    Service& operator=(const Service&) = delete;
    Service(Service&&)                 = delete;
    Service& operator=(Service&&)      = delete;

    /**
     * When higher level code is ready for the CLI
     * to begin full operations, call this method,
     * which will post the appropriate message
     * to the AO to get the CLI up and running.
     *
     * @param charDevice - the character device to use
     */
    void BeginCliAsync(cms::interfaces::CharacterDevice* charDevice);

    /**
     * Asynchronously add a CLI command binding to the embedded-cli
     * managed by this AO.
     *
     * Will assert if the AO is not active.
     * Will assert if no free bindings are available.
     *
     * @param binding
     */
    void AddCliBindingAsync(const CommandBinding& binding);

    /**
     *   Will asynchronously stop and release all CLI resources.
     */
    void EndCliAsync();

private:
    enum InternalSignals {
        BEGIN_CLI_SIG = CMS_EMBEDDED_CLI_SIGNAL_RANGE_START,
        END_CLI_SIG,
        NEW_CLI_DATA_SIG,
        ADD_CLI_BINDING_SIG,
        INTERNAL_MAX_SIG
    };
    static_assert(INTERNAL_MAX_SIG <= CMS_EMBEDDED_CLI_SIGNAL_RANGE_END,
                  "Assigned signal range must be increased");

    class BeginEvent : public QP::QEvt {
    public:
        cms::interfaces::CharacterDevice* mCharDevice;
    };

    class NewDataEvent : public QP::QEvt {
    public:
       uint8_t mByte;
    };

    class AddCliBindingEvent : public QP::QEvt {
    public:
        CommandBinding mBinding;
    };

    //Active Object States
    Q_STATE_DECL(initial);
    Q_STATE_DECL(inactive);
    Q_STATE_DECL(active);

    static void CliWriteChar(EmbeddedCli *embeddedCli, char c);
    static void NewByteReceived(void* userData, uint8_t byte);

    cms::interfaces::CharacterDevice* mCharacterDevice;

    //avoid pulling in embedded-cli header dependencies
    //this also in-theory allows for multiple CLI AO instances
    //an internal static_assert protects against future size changes
    //versus the embedded-cli struct.
    std::array<uint8_t, 32> mEmbeddedCliConfigBacking;

    //always points to the backing memory above
    EmbeddedCliConfig * const mEmbeddedCliConfig;
    EmbeddedCli * mEmbeddedCli;
    const Event mActiveEvent;
};

} //namespace EmbeddedCli
} //namespace cms

#endif   // CMS_EMBEDDED_CLI_SERVICE_HPP
