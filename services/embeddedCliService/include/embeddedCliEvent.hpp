//
// Created by eshlemanm on 6/16/24.
//

#ifndef EMBEDDED_CLI_FOR_QPCPP_EMBEDDEDCLIEVENT_HPP
#define EMBEDDED_CLI_FOR_QPCPP_EMBEDDEDCLIEVENT_HPP

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

#endif   // EMBEDDED_CLI_FOR_QPCPP_EMBEDDEDCLIEVENT_HPP
