# foxBMS Change Log


## Release 0.5.2

Release notes:
We fixed a bug in the ltc driver, leading to a non-functional temperature 
sensing for foxBMS Slave Hardware version 1.xx. The slave version is 
configuration for the primary MCU in foxBMS-primary\src\module\config\ltc_cfg.h
by the define SLAVE_BOARD_VERSION and for the secondary MCU in 
foxBMS-secondary\src\module\config\ltc_cfg.h by the define SLAVE_BOARD_VERSION.
 - Set SLAVE_BOARD_VERSION to "1" if you are using version 1.xx of the foxBMS 
   Slave.
 - Set SLAVE_BOARD_VERSION to "2" if you are using version 2.xx of the foxBMS 
   Slave. Version 2.xx is the default configuration.

Changelog :
- foxBMS-primary
  - fixed LTC temperature sensing bug
- foxBMS-secondary
  - fixed LTC temperature sensing bug


## Release 0.5.1

- foxBMS-setup
  - added parameter '-u', '--update' to bootstrap.py for updating the setup
    repository.
- foxBMS-primary
  - updates for waf 1.9.13 support
  - updated module/EEPROM and migrated to module/nvmram
  - minor code adaptations and cleanup
- foxBMS-secondary
  - support for waf 1.9.13
  - minor code adaptations and cleanup
- foxbMS-tools
  - updated waf from version 1.8.12 to version 1.9.13


## Release 0.5.0

A new project structure is now used by foxBMS. The documentation is no more
contained in the embedded software sources and has its own repository. FreeRTOS
and hal have their own repository, too.
A central repository called foxBMS-setup is now used. It contains
several scripts:
 - bootstrap.py gets all the repositories needed to work with foxBMS
 - build.py is used to compile binaries and to generate the documentation
 - clean.py is used to removed the generated binaries and documentation

Release notes:

 - New project structure

 - Added support for external (SPI) EEPROM on the BMS-Master
 - Redesign of can and cansignal module to simplify the usage
 - Added support for triggered and cyclic current measurement of Isabellenhütte
   current sensor (IVT)
 - Current sensor now functions by default in non-triggered modus (no
   reprogramming needed for the sensor)

 - Updated and restructured complete documentation
 - Restructured file and folder structure for the documentation
 - Added safety and risk analysis section
 - Cleaning up of non-used files in the documentation
 - Consistency check and correction of the naming and wording used
 - Addition of the source files (e.g., Microsoft Visio diagrams) used to
   generate the figures in the documentation
 - Reformatted the licenses text formatting (no changes in the licenses
   content)
 - Updated the battery junction box (BJB) section with up-to-date components
   and parameters


## Release 0.4.4

The checksum tool is now automatically called when building binaries.
Therefore the command
    python tools/waf-1.8.12 configure build chksum
is NOT longer supported. The command to build binaries with checksum support is
    python tools/waf-1.8.12 configure build
This is the build command used in foxBMS FrontDesk, that is, FrontDesk software
is compatible with this change and now supports automatic checksum builds.

Release notes:
 - Improved checksum-feature
 - Updated copyright 2010 - 2017


## Release 0.4.3

Starting from this version, a checksum mechanism was implemented in foxBMS. If
the checksum is active and it is not computed correctly, it will prevent the
flashed program from running. Details on deactivating the checksum can be found
in the Software FAQ, in How to use and deactivate the checksum.

Release notes:
 - Important: Changed contactor configuration order in the software to match
   the labels on the front
  - Contactor 0: CONT_PLUS_MAIN
  - Contactor 1: CONT_PLUS_PRECHARGE
  - Contactor 2: CONT_MINUS_MAIN
 - Fixed an bug which could cause an unintended closing of the contactors after
   recovering from error mode
 - Increased stack size for the engine tasks to avoid stack overflow in some
   special conditions
 - Added a note in the documentation to indicate the necessity to send a
   periodic CAN message to the BMS
 - Fixed DLC of CAN message for the current sensor measurement
 - Added checksum verification for the flashed binaries
 - Updated linker script to allow integration of the checksum tool
 - Activated debug without JTAG interface via USB


## Release 0.4.2

Release notes:
 - Removed schematic files from documentation, registration needed to obtain
   the files
 - Added entries to the software FAQ


## Release 0.4.1

Release notes:

 - Corrected daisy chain connector pinout in quickstart guide
 - Corrected code for contactors, to allow using contactors without feedback
 - Corrected LTC code for reading balancing feedback
 - Quickstart restructured, with mention of the necessity to generate the HTML
   documentation


## Release 0.4.0

Beta version of foxBS that was supplied to selected partners for evaluation.

Release notes: