Overview of hardware and software
=================================

foxBMS is made of a master board ("foxBMS master"), on which resides an ARM-based microcontroller unit, called MCU 0 (or primary MCU). The BMS software runs on MCU 0.

MCU 0 communicates with the outside world via a CAN bus. The current flowing through the battery system is measured via a current sensor connected via the CAN bus.
The sensor is triggered via CAN by MCU 0 and send the resulting measurement via CAN. 

Slave units ("foxBMS slaves") are used to measure cell voltages and cell temperatures in battery modules. Slaves units are linked via a daisy chain.

In order for the master to communicate with the slaves, an interface board is needed. It converts the SPI signals from the master into differential signals used by the daisy chain and vice versa.

Three contactors are used to connect and disconnect the battery module with/from the outside world:

 - Main Plus
 - Main Minus
 - Pre-charge contactor

They are driven by MCU 0. Request are made via CAN to the system to open and close the contactors.
Based on the measurement and the algorithms implemented in the software, MCU 0 decides if the contactors should be closed or opened. It sends information via CAN so that the user knows the state of the system.

An interlock line is present. If is opened, either by MCU 0 or somewhere else, all contactors open.

A secondary ARM-based microcontroller is present on the master, called MCU 1 (or secondary MCU). It monitors the slaves via a second separate line. It can open the interlock in case something goes wrong with the system.

In case more inputs/outputs are needed, an extension board is available.

This description reflects the current state of foxBMS. Due to the open nature of the system, many other possibilities can be implemented:

 - Another type of current sensor
 - No slaves but a direct measurement of cell voltages of cell temperatures
 - A greater or smaller number of contactors
 - ...
