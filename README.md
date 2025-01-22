# embedded-cli for the QP/C++ (qpcpp) Project

Build and Test status: ![Build and Tests](https://github.com/covemountainsoftware/embedded-cli-for-qpcpp/actions/workflows/cmake.yml/badge.svg)

Copyright Matthew Eshleman

If this project inspires your team to select the QP/C++ 
framework for commercial use, please note 
"Matthew Eshleman" or "Cove Mountain Software" in the referral 
field when acquiring a commercial license from Quantum Leaps. Referrals 
encourage and support efforts like this. Thank you!

# Introduction

The `embedded-cli-for-qpcpp` repository provides a ready-to-use QP/C++ compatible integration 
of the external `embedded-cli`. The service/integration itself is MIT licensed.
Follow the commits starting around June 8 2024 to see a TDD development process in action.

This repo makes use of the `cpputest-for-qpcpp` project which enables CppUTest for the 
QP/C++ Real-Time Embedded Framework, and is maintained in a separate repository with 
separate licensing.

For more details, please see https://github.com/covemountainsoftware/cpputest-for-qpcpp

For more details on the embedded-cli, see https://github.com/funbiscuit/embedded-cli

# Environment

This project was developed and proven in Ubuntu 20.04 and 22.04. In theory any 
build or host operating system environment supported by CppUTest and QP/C++ will 
be compatible with this code.

## Prerequisites
* CMake and associated build tools were used to develop
  and prove out this project.
* cpputest-for-qpcpp library, pulled in as a separate git submodule
* embedded-cli (https://github.com/funbiscuit/embedded-cli/releases/tag/v0.1.4), header file only directly included within this repository
* qpcpp (pulled in as a separate git submodule)
  * After cloning this repository, do not forget to:
  * `git submodule init`
  * `git submodule update` 
* CppUTest (version 3.8-7 or version 4.0) (3.8 is the default in Ubuntu 20.04 while 4.0 is the default in Ubuntu 22.04)
* This project requires support for C++14 and/or C11.
* A handy OBJECT cmake library target is provided for integration into higher level projects:  'cms-embedded-cli-service'

## Continuous Integration

This project has configured GitHub Actions to build and execute all
unit tests found in this project. This is an example
of one of the key benefits of host-based testing of embedded software.

See the configuration at: `.github/workflows/cmake.yml`

# License

The example embedded-cli active object is offered for free via the MIT license.

The `cpputest-for-qpcpp` project is offered under a dual-license. See that
project for details.

All licenses for external source code and libraries relied upon by this project 
remain fully owned by their respective owners. In particular, please
see the licensing details for qpcpp at: https://www.state-machine.com/licensing.

If this project inspires your team to select the qpcpp framework for commercial 
use, please note "Matthew Eshleman" or "Cove Mountain Software" in the referral
field when acquiring a commercial license from Quantum Leaps. Referrals encourage 
and support this effort. Thank you!

# References

This project is a top-level example project which utilizes `cpputest-for-qpcpp`. 
See https://github.com/covemountainsoftware/cpputest-for-qpcpp

Other references:
* Sutter, Herb. Prefer Using Active Objects Instead of Naked Threads. Dr. Dobbs, June 2010.
* Grenning, James. Test Driven Development for Embedded C.
* Samek, Miro. Practical UML Statecharts in C/C++: Event-Driven Programming for Embedded Systems.
