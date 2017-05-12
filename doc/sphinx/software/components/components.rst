.. include:: ../../macros.rst


===================
Software Components
===================

.. highlight:: C

The |foxbms| software is made out of the following components:

 - ``application``
 - ``engine``
 - ``general``
 - ``hal``
 - ``module``
 - ``os``

Application
-----------

The ``application`` directory contains the user applications.

+-------------+-----------------------------------------------------------------------------------+
| Element     | Description                                                                       |
+=============+===================================================================================+
| config      | Contains the configuration for the user applications (e.g., task configuration)   |
+-------------+-----------------------------------------------------------------------------------+
| sox         | Coulomb-counter (current integrator) and State-of-Function calculator             |
+-------------+-----------------------------------------------------------------------------------+
| task        | User specific cyclic tasks (10ms and 100ms)                                       |
+-------------+-----------------------------------------------------------------------------------+

Engine
------

The ``engine`` directory contains all the core functions of the BMS.

+-------------+------------------------------------------------------------------------------------+
| Element     | Description                                                                        |
+=============+====================================================================================+
| bmsctrl     | Decision are taken here by the BMS (e.g., open contactors in case of a problem)    |
+-------------+------------------------------------------------------------------------------------+
| config      | Contains the configuration of engine components (e.g., task configuration)         |
+-------------+------------------------------------------------------------------------------------+
| database    | Implementation of the asynchronous data exchange                                   |
+-------------+------------------------------------------------------------------------------------+
| diag        | With this software module, other modules can report problems                       |
+-------------+------------------------------------------------------------------------------------+
| sysctrl     | Manages the power contactors                                                       |
+-------------+------------------------------------------------------------------------------------+
| task        | Cyclic engine tasks (1, 10 and 100ms) that call system related functions           |
+-------------+------------------------------------------------------------------------------------+

Application tasks should be used to call user-defined functions. 

General
-------

The ``general`` directory contains the main function and configuration files.

+-------------+-----------------------------------------------------------------------------------+
| Element     | Description                                                                       |
+=============+===================================================================================+
| main.c      | Initialization of hardware modules, of interrupts and of the operating system     |
+-------------+-----------------------------------------------------------------------------------+
| config      | Contains the configuration for the system initialization:                         |
|             | configuration and interface functions to HAL and FreeRTOS,                        |
|             | global definitions, interrupt configurations and startup code                     |
+-------------+-----------------------------------------------------------------------------------+
| nvic.c      | interrupt initialization                                                          |
+-------------+-----------------------------------------------------------------------------------+
| includes    | Contains the standard types                                                       |
+-------------+-----------------------------------------------------------------------------------+

HAL
---

``hal`` contains the Hardware Abstraction Layer. It is used by the system but is provided by the MCU manufacturer, in this case ST-Microelectronics. It is used by |foxbms| but not part of |foxbms|.

+-----------------------+--------------------------------------------------------------------------------+
| Element               | Description                                                                    |
+=======================+================================================================================+
| CMSIS                 | Interface and configuration of CMSIS                                           |
+-----------------------+--------------------------------------------------------------------------------+
| STM32F4xx_HAL_Driver  | STM32F4xx family Hardware Abstraction Layer drivers                            |
+-----------------------+--------------------------------------------------------------------------------+

Module
------

The ``module`` directory contains all the software modules needed by the BMS.

+-------------+-------------------------------------------------------------------------------------------+
| Element     | Description                                                                               |
+=============+===========================================================================================+
| adc         | Driver for analog to digital converter, measurement of lithium backup battery voltage     |
+-------------+-------------------------------------------------------------------------------------------+
| can         | Driver to receive/transmit CAN message                                                    |
+-------------+-------------------------------------------------------------------------------------------+
| cansignal   | Definition of CAN messages and signals                                                    |
+-------------+-------------------------------------------------------------------------------------------+
| chksum      | Checksum algorithms for modulo 32-bit addition and CRC32                                  |
+-------------+-------------------------------------------------------------------------------------------+
| com         | Serial port communication layer (for debug purposes)                                      |
+-------------+-------------------------------------------------------------------------------------------+
| config      | Contains the configuration for the software modules                                       |
+-------------+-------------------------------------------------------------------------------------------+
| contactor   | Driver to open/close contactors and read contactor feedback                               |
+-------------+-------------------------------------------------------------------------------------------+
| dma         | Configuration for Direct Memory Access (e.g. used for SPI Communication)                  |
+-------------+-------------------------------------------------------------------------------------------+
| eeprom      | Driver for non-volatile storage of data. Retains data even if the 3V button cell fails    |
+-------------+-------------------------------------------------------------------------------------------+
| intermcu    | Driver for communication between |MCU0| (primary) and |MCU1| (secondary)                  |
+-------------+-------------------------------------------------------------------------------------------+
| io          | Driver and interfaces for I/O ports (control of output pins and read of input pins)       |
+-------------+-------------------------------------------------------------------------------------------+
| isoguard    | Driver for monitoring galvanic isolation in the system                                    |
+-------------+-------------------------------------------------------------------------------------------+
| ltc         | Driver for battery cell monitoring IC                                                     |
+-------------+-------------------------------------------------------------------------------------------+
| mcu         | MCU-dependent low-level functions (specific registers, MCU timestamps)                    |
+-------------+-------------------------------------------------------------------------------------------+
| rcc         | Configuration of the prescaler for the MCU clock system                                   |
+-------------+-------------------------------------------------------------------------------------------+
| rtc         | Real time clock driver, Control and Access of Backup SRAM Registers                       |
+-------------+-------------------------------------------------------------------------------------------+
| spi         | Driver for communication via Serial Peripheral Interface (SPI bus)                        |
+-------------+-------------------------------------------------------------------------------------------+
| timer       | Driver for MCU timer peripherals                                                          |
+-------------+-------------------------------------------------------------------------------------------+
| uart        | Driver for serial communication (UART, RS232 , RS485)                                     |
+-------------+-------------------------------------------------------------------------------------------+
| utils       | Contains utilities like the LED blink driver                                              |
+-------------+-------------------------------------------------------------------------------------------+
| watchdog    | Driver for the watchdog timer                                                             |
+-------------+-------------------------------------------------------------------------------------------+

OS
--

The ``os`` directory contains configurations for the operating system.

+-------------+----------------------------------------------------------------------------------------+
| Element     | Description                                                                            |
+=============+========================================================================================+
| FreeRTOS    | Operating System Software (FreeRTOS)                                                   |
+-------------+----------------------------------------------------------------------------------------+
| os.c        | Interface to FreeRTOS (e.g., wrapper functions of cyclic application and engine tasks) | 
+-------------+----------------------------------------------------------------------------------------+


