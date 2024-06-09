///***************************************************************************
/// @brief The public signals used in the publish/subscribe event system
///***************************************************************************

#ifndef PUB_SUB_SIGNALS_HPP
#define PUB_SUB_SIGNALS_HPP

#include "qpcpp.hpp"

/// The publish/subscribe signals allocated for this project
enum PubSubSignals {
    STARTING_PUB_SUB_SIG = QP::Q_USER_SIG,

    #include "embeddedCliServicePubSubSignals.hpp"

    MAX_PUB_SUB_SIG   // the last published signal
};

#endif   // PUB_SUB_SIGNALS_HPP
