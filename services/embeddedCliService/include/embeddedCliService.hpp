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

namespace cms {
namespace EmbeddedCli {

class Service final : public QP::QActive {
public:
    Service();
    ~Service() = default;

    Service(const Service&)            = delete;
    Service& operator=(const Service&) = delete;
    Service(Service&&)                 = delete;
    Service& operator=(Service&&)      = delete;

    /**
     *
     */


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
        BEGIN_CLI = PubSub::MAX_PUB_SIG
    };

    class BeginEvent : public QP::QEvt {
    public:
        cms::interfaces::CharacterDevice* m_charDevice;

        explicit BeginEvent(enum_t sig, cms::interfaces::CharacterDevice* charDevice) :
            QP::QEvt(sig),
            m_charDevice(charDevice)
        {
        }
    };

    Q_STATE_DECL(initial);
    Q_STATE_DECL(inactive);
};

} //namespace EmbeddedCli
} //namespace cms

#endif   // EMBEDDED_CLI_FOR_QPCPP_EMBEDDEDCLISERVICE_HPP
