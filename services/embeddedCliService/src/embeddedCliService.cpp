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
    static const QP::QEvt inactiveEvent = QP::QEvt(PubSub::EMBEDDED_CLI_INACTIVE_SIG);

    QP::QState rtn;
    switch (e->sig) {
        case Q_ENTRY_SIG:
            QP::QF::PUBLISH(&inactiveEvent, this);
            rtn = Q_RET_HANDLED;
            break;
        case BEGIN_CLI_SIG: {
            auto beginEvent  = reinterpret_cast<const BeginEvent*>(e);
            mCharacterDevice = beginEvent->m_charDevice;
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
    QP::QState rtn;
    switch (e->sig) {
        case Q_ENTRY_SIG:
            mEmbeddedCliConfig = embeddedCliDefaultConfig();
            mEmbeddedCli = embeddedCliNew(mEmbeddedCliConfig);
            mEmbeddedCli->appContext = this;
            mEmbeddedCli->writeChar = &Service::CliWriteChar;
            embeddedCliProcess(mEmbeddedCli);
            rtn = Q_RET_HANDLED;
            break;
        case BEGIN_CLI_SIG:
            //we are already active, drop this Begin request
            rtn = Q_RET_HANDLED;
            break;
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
    e->m_charDevice = charDevice;
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

} //namespace EmbeddedCLI
} //namespace cms
