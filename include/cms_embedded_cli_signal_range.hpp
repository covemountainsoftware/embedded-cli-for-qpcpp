//
// This header is an example header demonstrating where the
// containing firmware project assigns the range of signal
// values assigned to the target service's internal or posted signals.
// In this case, we are simply assigning a range of 10 starting immediately
// following the max pub/sub signal value. Other firmware projects may
// wish to assign distinct ranges to each active object or service.
//

#ifndef EMBEDDED_CLI_FOR_QPCPP_CMS_EMBEDDED_CLI_SIGNAL_RANGE_HPP
#define EMBEDDED_CLI_FOR_QPCPP_CMS_EMBEDDED_CLI_SIGNAL_RANGE_HPP

#include "pubsub_signals.hpp"

constexpr enum_t CMS_EMBEDDED_CLI_SIGNAL_RANGE_START = MAX_PUB_SUB_SIG + 1;
constexpr enum_t CMS_EMBEDDED_CLI_SIGNAL_RANGE_END =
                                   CMS_EMBEDDED_CLI_SIGNAL_RANGE_START + 10;

#endif   // EMBEDDED_CLI_FOR_QPCPP_CMS_EMBEDDED_CLI_SIGNAL_RANGE_HPP
