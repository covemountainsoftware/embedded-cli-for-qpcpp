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
#include "cms_pubsub.hpp"
#include "qsafe.h"
#include "embedded_cli.h"

Q_DEFINE_THIS_MODULE("EmbeddedCliService")

namespace cms {
namespace EmbeddedCLI {

Service::Service(uint64_t* buffer, size_t bufferElementCount, uint16_t maxBindingCount, const char * customInvitation) :
    QP::QActive(initial),
    mCharacterDevice(nullptr),
    mEmbeddedCliConfigBacking(),
    mEmbeddedCliConfig(reinterpret_cast<EmbeddedCliConfig*>(mEmbeddedCliConfigBacking.data())),
    mEmbeddedCli(nullptr),
    mActiveEvent(CMS_EMBEDDED_CLI_ACTIVE_SIG, this)
{
    static_assert(sizeof(mEmbeddedCliConfigBacking) >= sizeof(EmbeddedCliConfig),
                  "backing memory for the cli config is not large enough!");

    //one time config setup during construction. Saves on member
    //variable storage too.
    *mEmbeddedCliConfig = *embeddedCliDefaultConfig();

    if (buffer != nullptr) {
        mEmbeddedCliConfig->cliBuffer = buffer;
        mEmbeddedCliConfig->cliBufferSize = bufferElementCount * sizeof(uint64_t);
    }

    if (customInvitation != nullptr) {
        mEmbeddedCliConfig->invitation = customInvitation;
    }

    if (maxBindingCount != 0) {
        mEmbeddedCliConfig->maxBindingCount = maxBindingCount;
    }

    // After all configuration items have been set,
    // make sure our static buffer is large enough, but
    // only if we are using that option.
    if (buffer != nullptr) {
        uint16_t requiredSize = embeddedCliRequiredSize(mEmbeddedCliConfig);
        Q_ASSERT(requiredSize <= bufferElementCount * sizeof(uint64_t));
    }
}

Service::~Service()
{
    if (mEmbeddedCli)
    {
        embeddedCliFree(mEmbeddedCli);
    }

    mEmbeddedCli = nullptr;
    mCharacterDevice = nullptr;
}

Q_STATE_DEF(Service, initial)
{
    (void)e;
    return tran(&inactive);
}

Q_STATE_DEF(Service, inactive)
{
    static const QP::QEvt inactiveEvent = QP::QEvt(CMS_EMBEDDED_CLI_INACTIVE_SIG);

    QP::QState rtn;
    switch (e->sig) {
        case Q_ENTRY_SIG:
            if (mEmbeddedCli)
            {
                embeddedCliFree(mEmbeddedCli);
            }
            mEmbeddedCli = nullptr;
            mCharacterDevice = nullptr;
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
        case ADD_CLI_BINDING_SIG:
            Q_ASSERT(false);
            rtn = Q_RET_HANDLED;
            break;
        case END_CLI_SIG:
            //nothing to do, already inactive
            rtn = Q_RET_HANDLED;
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
        case Q_ENTRY_SIG: {
            mCharacterDevice->RegisterNewByteCallback(NewByteReceived, this);
            mEmbeddedCli = embeddedCliNew(mEmbeddedCliConfig);
            Q_ASSERT(mEmbeddedCli != nullptr);

            mEmbeddedCli->appContext = this;
            mEmbeddedCli->writeChar = &Service::CliWriteChar;
            embeddedCliProcess(mEmbeddedCli);
            QP::QF::PUBLISH(&mActiveEvent, this);
            rtn = Q_RET_HANDLED;
            break;
        }
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
        case ADD_CLI_BINDING_SIG: {
            auto addBindingEvent = reinterpret_cast<const AddCliBindingEvent*>(e);
            CliCommandBinding binding;

            static_assert(sizeof(CliCommandBinding) == sizeof(CommandBinding), "internal compatibility may have changed");

            binding.context = addBindingEvent->mBinding.context;
            binding.binding = addBindingEvent->mBinding.binding;
            binding.name = addBindingEvent->mBinding.name;
            binding.help = addBindingEvent->mBinding.help;
            binding.tokenizeArgs = addBindingEvent->mBinding.tokenizeArgs;
            bool ok = embeddedCliAddBinding(mEmbeddedCli, binding);
            Q_ASSERT(ok);
            embeddedCliProcess(mEmbeddedCli);
            rtn = Q_RET_HANDLED;
            break;
        }
        case END_CLI_SIG:
            rtn = tran(&inactive);
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
    e->mCharDevice = charDevice;
    this->POST(e, 0);
}

void Service::EndCliAsync()
{
    static const QP::QEvt endCliEvent = QP::QEvt(END_CLI_SIG);
    this->POST(&endCliEvent, 0);
}

void Service::AddCliBindingAsync(const CommandBinding& binding)
{
    Q_ASSERT(binding.binding != nullptr);
    Q_ASSERT(binding.name != nullptr);
    auto e = Q_NEW(AddCliBindingEvent, ADD_CLI_BINDING_SIG);
    e->mBinding = binding;
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
