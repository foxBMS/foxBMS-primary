==================
foxBMS
==================

foxBMS is a free, open and flexible development environment for the design of battery management systems. It is the first universal BMS development environment.

The foxBMS-primary and foxBMS-secondary repositories contain the C-code of the two Microcontroller Units (MCU) located on the foxBMS-Master board. The other foxBMS electronic boards do not contain any software.

The foxBMS-hardware repository contains the layout and schematic files of the foxBMS hardware (i.e., foxBMS-Master, foxBMS-Extension, foxBMS-Interface, foxBMS-Slave).

The foxConda-installer repository contains the installer for the foxConda environment. This environment provides all the tools necessary to generate the documentation, compile the code for the MCUs and flash the generated binaries on the MCUs.

The foxBMS-documentation repository contains the Sphinx and Doxygen documentation. It can also be found in the foxBMS-primary repository, in the doc/sphinx and doc/doxygen directories. A generated version of the Sphinx documentation can be found at http://foxbms.readthedocs.io/en/latest/. It explains the structure of the foxBMS hardware, how to install the foxConda environment and how to use foxConda to compile and flash the sources.
