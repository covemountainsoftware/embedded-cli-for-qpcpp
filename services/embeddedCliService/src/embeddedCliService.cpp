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

#include "embeddedCliService.hpp"
#include "pubsub_signals.hpp"
#include "qsafe.h"

//include the embedded-cli with implementation flag,
//which pulls the actual CLI implementation into this source
//code file.
#define EMBEDDED_CLI_IMPL
#include "embedded_cli.h"

Q_DEFINE_THIS_MODULE("EmbeddedCliService")

namespace cms {
namespace EmbeddedCLI {

Service::Service() :
    QP::QActive(initial),
    mCharacterDevice(nullptr),
    mEmbeddedCliConfig(nullptr),
    mEmbeddedCli(nullptr)
{
}

Service::~Service()
{
    if (mEmbeddedCli)
    {
        embeddedCliFree(mEmbeddedCli);
    }

    mEmbeddedCli = nullptr;
    mEmbeddedCliConfig = nullptr;
    mCharacterDevice = nullptr;
}

Q_STATE_DEF(Service, initial)
{
    (void)e;
    return tran(&inactive);
}

Q_STATE_DEF(Service, inactive)
{
    static const QP::QEvt inactiveEvent = QP::QEvt(EMBEDDED_CLI_INACTIVE_SIG);

    QP::QState rtn;
    switch (e->sig) {
        case Q_ENTRY_SIG:
            QP::QF::PUBLISH(&inactiveEvent, this);
            rtn = Q_RET_HANDLED;
            break;
        case BEGIN_CLI_SIG: {
            auto beginEvent  = reinterpret_cast<const BeginEvent*>(e);
            Q_ASSERT(beginEvent->mCharDevice != nullptr);
            mCharacterDevice = beginEvent->mCharDevice;
            rtn              = tran(&active);
        }
            break;
        default:
            rtn = super(&top);
            break;
    }

    return rtn;
}

Q_STATE_DEF(Service, active)
{
    static const QP::QEvt activeEvent = QP::QEvt(EMBEDDED_CLI_ACTIVE_SIG);

    QP::QState rtn;
    switch (e->sig) {
        case Q_ENTRY_SIG:
            mCharacterDevice->RegisterNewByteCallback(NewByteReceived, this);
            mEmbeddedCliConfig = embeddedCliDefaultConfig();
            mEmbeddedCli = embeddedCliNew(mEmbeddedCliConfig);
            mEmbeddedCli->appContext = this;
            mEmbeddedCli->writeChar = &Service::CliWriteChar;
            embeddedCliProcess(mEmbeddedCli);
            QP::QF::PUBLISH(&activeEvent, this);
            rtn = Q_RET_HANDLED;
            break;
        case BEGIN_CLI_SIG:
            //we are already active, drop this Begin request
            rtn = Q_RET_HANDLED;
            break;
        case NEW_CLI_DATA_SIG: {
            auto dataEvent = reinterpret_cast<const NewDataEvent*>(e);
            embeddedCliReceiveChar(mEmbeddedCli, static_cast<char>(dataEvent->mByte));
            embeddedCliProcess(mEmbeddedCli);
            rtn = Q_RET_HANDLED;
            break;
        }
        default:
            rtn = super(&top);
            break;
    }

    return rtn;
}

void Service::BeginCliAsync(cms::interfaces::CharacterDevice* charDevice)
{
    Q_ASSERT(charDevice != nullptr);
    auto e = Q_NEW(BeginEvent, BEGIN_CLI_SIG);
    e->mCharDevice = charDevice;
    this->POST(e, 0);
}

void Service::CliWriteChar(EmbeddedCli *embeddedCli, char c)
{
    auto me = static_cast<Service*>(embeddedCli->appContext);

    if ((me != nullptr) && (me->mCharacterDevice != nullptr))
    {
        me->mCharacterDevice->WriteAsync(static_cast<uint8_t>(c));
    }
    else
    {
        Q_ASSERT(me != nullptr);
        Q_ASSERT(me->mCharacterDevice != nullptr);
    }
}

void Service::NewByteReceived(void* userData, uint8_t byte)
{
    // This character device callback event could happen from
    // any thread or ISR context.
    //
    // In theory, the embedded-cli code supports incoming chars
    // from an ISR context. However, for maximum portability,
    // I'm going to just copy this byte and send it to the AO
    // for processing, mainly because the embedded-cli docs say:
    //
    //    "embeddedCliReceiveChar() can be called from normal code or
    //     from ISRs (but don't call it from multiple places)."
    //
    // That bit about not calling from multiple places gave me
    // pause. So, for maximum paranoia, will copy and send the
    // byte to the AO for processing.

    auto me = static_cast<Service*>(userData);
    if (me != nullptr)
    {
        auto e = Q_NEW(NewDataEvent, NEW_CLI_DATA_SIG);
        e->mByte = byte;
        me->POST(e, 0);
    }
    else
    {
        Q_ASSERT(userData != nullptr);
    }
}

} //namespace EmbeddedCLI
} //namespace cms
