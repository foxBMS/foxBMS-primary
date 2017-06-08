# foxBMS Change Log

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
 - Important: Changed contactor configuration order in the software to match the labels on the front
  - Contactor 0: CONT_PLUS_MAIN
  - Contactor 1: CONT_PLUS_PRECHARGE
  - Contactor 2: CONT_MINUS_MAIN
 - Fixed an bug which could cause an unintended closing of the contactors after recovering from error mode
 - Increased stack size for the engine tasks to avoid stack overflow in some special conditions
 - Added a note in the documentation to indicate the necessity to send a periodic CAN message to the BMS
 - Fixed DLC of CAN message for the current sensor measurement
 - Added checksum verification for the flashed binaries
 - Updated linker script to allow integration of the checksum tool
 - Activated debug without JTAG interface via USB

## Release 0.4.2

Release notes:
 - Removed schematic files from documentation, registration needed to obtain the files
 - Added entries to the software FAQ

## Release 0.4.1

Release notes:

 - Corrected daisy chain connector pinout in quickstart guide
 - Corrected code for contactors, to allow using contactors without feedback
 - Corrected LTC code for reading balancing feedback
 - Quickstart restructured, with mention of the necessity to generate the HTML documentation

## Release 0.4.0
Beta version of foxBS that was supplied to selected partners for evaluation.

Release notes: