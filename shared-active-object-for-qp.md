# Introduction

This document provides information on this repository's adherence 
to the guidelines noted at: ["Shared Active Object for QP Best Practices"](https://github.com/covemountainsoftware/Shared-Active-Objects-For-QP-Best-Practices)

# Section A, General
****
* A.1 Shared QP Readme:  Yes. This file.
* A.2 Source Code:  Yes, this repository.
* A.3 License: Yes, see LICENSE.txt.
* A.4 Unit tests. Yes, all included.
* A.5 Published Signals: Yes, see embeddedCliServicePubSubSignals.hpp
* A.6 Subscribed Signals: n/a.  No signals are subscribed.
* A.7 Posted Signals: n/a. No posted signals, rather methods are 
                       provided which internally post using private signals.
* A.8 QP Timers: none
* A.9 Message Pool: See integration.
* A.10 Microcontroller resources. See third-party embedded CLI requirements.
* A.11 Interfaces. Yes, integrator must create a character device 
                    driver, see characterDeviceInterface.hpp
* A.12 Interface Ownership: The embedded CLI expects to own a single 
                             character device, such as a single UART.
* A.13 Example project:  A linux based QP example project is provided.
                          A microcontroller example is not provided at this time.

# Section B, Build System

* B.1 Working Build:  Yes, known to build in Ubuntu 22.04 environment. 
                      The following files are of interest if not using
                      the provided CMake build files:
  * drivers/characterDevice/include/characterDeviceInterface.hpp
  * services/embeddedCliService/include/*.hpp
  * services/embeddedCliService/src/embedded_cli_impl.c
  * services/embeddedCliService/src/embeddedCliService.cpp
* B.2 Prefer CMake: This library provides CMake build files.
* B.3 Static library:  Not provided given just two source files to build.
* B.4 Build Configuration Options: no options specific to this module.
* B.5 Consistent Prefix:  "cms" or "CMS" via C++ namespace and all public signals.
* B.6 QP/Spy:  Not provided at this time.

# Section C, Containing Project Responsibilities

* C.1 Publish Subscribe header:  This project expects a "cms_pubsub.hpp" header to be present
                                 in the build include search path.
* C.2 Manage Signal Ranges: See I.6.
* C.3 Concrete Interfaces:  integrator must provide a concrete implementation of 
                            the CharacterDevice interface.

# Section I, Integration

* I.1 Identifier:  This module falls within the covemountainsoftware (CMS) vendor domain 
                 and associated identifier.
* I.2 Publish/Subscribe Signals:  
          See services/embeddedCliService/include/embeddedCliServicePubSubSignals.hpp
* I.3 namespace usage:  This service uses the 'cms' namespace.
* I.4 Signal Enumeration Prefix: This service uses the 'CMS' prefix.
* I.5 Publish/Subscribe Signals Global Namespace:  Yes, this module expects that  
                                     pub/sub signals are in the global C++ namespace.
* I.6 Posted and Private Signal Ranges: This module expects to find the header
                                     "cms_embedded_cli_signal_range.hpp" in the 
                                     build include search path.
* I.7 Naming: C++ namespace 'cms' is primary means of ensuring no symbol clashes.
* I.8 Static and global variables:  none.
* I.9 Discovery: No discovery provided at this time. However, the service publishes a
                 CMS_EMBEDDED_CLI_ACTIVE_SIG, of event message type cms::EmbeddedCLI::Event,
                 which includes a pointer to the service which just went active. Users
                 of this service can then use that pointer to register any desired CLI
                 commands.
* I.10 Initialization Behavior:  The service starts in an idle state and requires an external
                                 user to call BeginCliAsync(...) with a concrete character device.
* I.11 Priority: No guidance provided. Typically, the CLI is a low priority active object.

                      