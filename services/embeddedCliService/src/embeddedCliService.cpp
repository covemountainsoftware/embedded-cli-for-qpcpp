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

//include the embedded-cli with implementation flag,
//which pulls the actual CLI implementation into this source
//code file.
#define EMBEDDED_CLI_IMPL
#include "embedded_cli.h"

namespace cms {
namespace EmbeddedCli {

Service::Service() :
    QP::QActive(initial)
{
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
        default:
            rtn = super(&top);
            break;
    }

    return rtn;
}

void Service::BeginCliAsync()
{
    //todo
}

} //namespace EmbeddedCli
} //namespace cms