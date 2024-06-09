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

#include "qpcpp.hpp"
#include "pubsub_signals.hpp"
#include "characterDeviceInterface.hpp"

//forward declare the third-party EmbeddedCli structs
struct EmbeddedCli;
struct EmbeddedCliConfig;

namespace cms {
namespace EmbeddedCLI { //note, all caps CLI needed to avoid conflicts

class Service final : public QP::QActive {
public:
    Service();
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

private:
    enum InternalSignals {
        BEGIN_CLI_SIG = PubSub::MAX_PUB_SIG
    };

    class BeginEvent : public QP::QEvt {
    public:
        cms::interfaces::CharacterDevice* m_charDevice;
    };

    //Active Object States
    Q_STATE_DECL(initial);
    Q_STATE_DECL(inactive);
    Q_STATE_DECL(active);

    static void CliWriteChar(EmbeddedCli *embeddedCli, char c);

    cms::interfaces::CharacterDevice* mCharacterDevice;
    EmbeddedCliConfig * mEmbeddedCliConfig;
    EmbeddedCli * mEmbeddedCli;
};

} //namespace EmbeddedCli
} //namespace cms

#endif   // EMBEDDED_CLI_FOR_QPCPP_EMBEDDEDCLISERVICE_HPP
