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

private:
    Q_STATE_DECL(initial);
    Q_STATE_DECL(inactive);

};

} //namespace EmbeddedCli
} //namespace cms

#endif   // EMBEDDED_CLI_FOR_QPCPP_EMBEDDEDCLISERVICE_HPP
