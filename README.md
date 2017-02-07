==================
foxBMS
==================

foxBMS is a free, open and flexible development environment for the design of battery management systems. It is the first universal BMS development environment.

foxBMS is made out of two Microcontroller Units (MCU), named primary and secondary. The C code for the primary MCU is found in the repository foxBMS-primary. The C code for the secondary MCU is found in the repository foxBMS-secondary.

The layout and schematic files for the foxBMS hardware are found in the foxBMS-hardware repository.

The foxConda-installer repository contains the installer for the foxConda environment. This environment provides all the tools necessary to generate the documentation, compile the code for the MCUs and flash the generated binaries on the MCUs.

The documentation of foxBMS (Sphinx and Doxygen) is found in the foxBMS-primary repository, in the doc/sphinx and doc/doxygen directories. A generated version of the Sphinx documentation can be found at http://foxbms.readthedocs.io/en/latest/. It explains the structure of the foxBMS hardware, how to install the foxConda environment and how to use foxConda to compile and flash the sources.