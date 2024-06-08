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

    return tran(&common);
}

Q_STATE_DEF(Service, common)
{
    (void)e;

    //TODO

    return Q_RET_HANDLED;
}

} //namespace EmbeddedCli
} //namespace cms
