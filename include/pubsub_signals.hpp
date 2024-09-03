///***************************************************************************
/// @brief The public signals used in the publish/subscribe event system
///         This is an example of what a larger firmware project may have
///         and includes a few unused signals to illustrate how integration
///         of the CLI module is achieved.
///***************************************************************************

#ifndef PUB_SUB_SIGNALS_HPP
#define PUB_SUB_SIGNALS_HPP

#include "qpcpp.hpp"

/// The publish/subscribe signals allocated for this project
enum PubSubSignals {
    STARTING_PUB_SUB_SIG = QP::Q_USER_SIG,

    SOME_OTHER_SIG,

    #include "embeddedCliServicePubSubSignals.hpp"

    YET_ANOTHER_SIG,
    YET_MORE_B_SIG,

    MAX_PUB_SUB_SIG   // the last published signal
};

#endif   // PUB_SUB_SIGNALS_HPP
