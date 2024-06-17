/// @brief  The Embedded-CLI Service, Event class
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

#ifndef CMS_EMBEDDED_CLI_EVENT_HPP
#define CMS_EMBEDDED_CLI_EVENT_HPP

#include "qpcpp.hpp"
#include "embeddedCliService.hpp"

namespace cms {
namespace EmbeddedCLI {

/**
 * A trivial event with a convenience member holding
 * a pointer to the Embedded CLI Service originating
 * the event.
 */
class Event : public QP::QEvt {
public:
    explicit constexpr Event(enum_t s) noexcept
        : QP::QEvt(s),
        mCliService(nullptr)
    {
    }

    explicit constexpr Event(enum_t s, Service* service) noexcept
        : QP::QEvt(s),
        mCliService(service)
    {
    }

    Service* mCliService;
};

} //namespace EmbeddedCLI
} //namespace cms

#endif   // CMS_EMBEDDED_CLI_EVENT_HPP
