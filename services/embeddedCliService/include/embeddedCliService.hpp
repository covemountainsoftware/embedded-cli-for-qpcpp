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

#ifndef EMBEDDED_CLI_FOR_QPCPP_EMBEDDEDCLISERVICE_HPP
#define EMBEDDED_CLI_FOR_QPCPP_EMBEDDEDCLISERVICE_HPP

#include <cstdint>
#include <cstddef>
#include <array>
#include "qpcpp.hpp"
#include "pubsub_signals.hpp"
#include "characterDeviceInterface.hpp"

//forward declare the third-party EmbeddedCli structs
struct EmbeddedCli;
struct EmbeddedCliConfig;

namespace cms {
namespace EmbeddedCLI { //note, all caps CLI needed to avoid conflicts

/**
 * This is basically a copy of the same struct from embedded-cli.
 * Going a bit out of my way to avoid exposing embedded-cli details
 * outside this service.
 */
struct CommandBinding {
    /**
     * Name of command to bind. Should not be NULL.
     */
    const char *name;

    /**
     * Help string that will be displayed when "help <cmd>" is executed.
     * Can have multiple lines separated with "\r\n"
     * Can be NULL if no help is provided.
     */
    const char *help;

    /**
     * Flag to perform tokenization before calling binding function.
     */
    bool tokenizeArgs;

    /**
     * Pointer to any specific app context that is required for this binding.
     * It will be provided in binding callback.
     */
    void *context;

    /**
     * Binding function for when command is received.
     * @param cli - pointer to cli that is calling this binding
     * @param args - string of args (if tokenizeArgs is false) or tokens otherwise
     * @param context
     */
    void (*binding)(EmbeddedCli *cli, char *args, void *context);
};

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
     * @param customInvitation - a custom string for the CLI prompt.
     *                           Set to nullptr for the internal default prompt
     */
    explicit Service(uint64_t* buffer, size_t bufferElementCount, const char * customInvitation);
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
     * @param buffer - pointer to a statically allocated buffer using
     *                 uint64. Must be aligned to uint64 boundaries too.
     * @param bufferElementCount - the number of uint64 elements
     *                             available at 'buffer'
     *
     * @note: If 'buffer' is nullptr, will use the embedded-cli's default configuration
     *        which will allocate memory via malloc.
     * @note: Using uint64 to ensure all embedded-cli alignment requirements
     *        are met, regardless of environment.
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

private:
    enum InternalSignals {
        BEGIN_CLI_SIG = MAX_PUB_SUB_SIG,
        NEW_CLI_DATA_SIG
    };

    class BeginEvent : public QP::QEvt {
    public:
        cms::interfaces::CharacterDevice* mCharDevice;
    };

    class NewDataEvent : public QP::QEvt {
    public:
       uint8_t mByte;
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
};

} //namespace EmbeddedCli
} //namespace cms

#endif   // EMBEDDED_CLI_FOR_QPCPP_EMBEDDEDCLISERVICE_HPP
