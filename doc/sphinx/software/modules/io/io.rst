IO
==

.. include:: ../../../macros.rst

.. highlight:: C

This user manual describes the configuration of the pins of the foxbms microcontroller using 
the |io module|. The |io module| allows an easy way to configure all pins of the microcontroller
at a central position, reading the input signals and writing signals to the output pins of 
the microcontroller.

.. ignore .. sectnum::

.. contents:: Table Of Contents

Module Files
~~~~~~~~~~~~

Driver:
 - src\\module\\io\\io.c
 - src\\module\\io\\io.h
 
Driver Configuration:
 - src\\module\\config\\io_cfg.c
 - src\\module\\config\\io_cfg.h
 - src\\module\\config\\io_mcu0.h
 - src\\module\\config\\io_package_cfg.h


Configuration Of The GPIOs
~~~~~~~~~~~~~~~~~~~~~~~~~~

Initialization Of The GPIOs
---------------------------

A short example shows how to configure pins at the CPU:
Starting point is the following pin configuration:


===========    ========    ==================================  ================================
Package Pin    Port-Pin    Signal name                         Alternative Function
===========    ========    ==================================  ================================
...
16             F-0         PIN_MCU_0_FMC_RAM_A0                flexible memory controller (FMC)
17             F-1         PIN_MCU_0_FMC_RAM_A1                flexible memory controller (FMC)
...
128            D-13        PIN_MCU_0_ISO_GPIO_IN_1             Digital Input
133            G-9         PIN_MCU_0_ISO_GPIO_OUT_0            Digital Output
...
===========    ========    ==================================  ================================

To define a signal one has to simply set a ``#define`` in ``io_cfg_foxbms.h`` describing the signal

.. code-block:: C
   
   ...
   #define PIN_MCU_0_FMC_RAM_A0               IO_PF_0
   #define PIN_MCU_0_FMC_RAM_A1               IO_PF_1
   ...
   #define PIN_MCU_0_ISO_GPIO_IN_1            IO_PD_13
   #define PIN_MCU_0_ISO_GPIO_OUT_0           IO_PG_9
   ...

The initialization of this configuration on the hardware is executed through the |io module|'s function ``IO_Init(*io_cfg)``.
One has to configure the ``const IO_PIN_CFG_s io_cfg[]`` in ``io_cfg.h`` which looks for this example like this

.. code-block:: C
   
   IO_PIN_CFG_s io_cfg[] = {
   {PIN_MCU_0_FMC_RAM_A0,      IO_MODE_AF_PP,       IO_PIN_NOPULL,    IO_SPEED_HIGH,    IO_ALTERNATE_AF12_FMC,        IO_PIN_LOCK_ENABLE},
   {PIN_MCU_0_FMC_RAM_A1,      IO_MODE_AF_PP,       IO_PIN_NOPULL,    IO_SPEED_HIGH,    IO_ALTERNATE_AF12_FMC,        IO_PIN_LOCK_ENABLE},
   ...
   {PIN_MCU_0_ISO_GPIO_IN_1,   IO_MODE_INPUT,       IO_PIN_PULLDOWN,  IO_SPEED_FAST,    IO_ALTERNATE_NO_ALTERNATE,    IO_PIN_LOCK_ENABLE},
   {PIN_MCU_0_ISO_GPIO_OUT_0,  IO_MODE_OUTPUT_PP,   0,                0,                IO_ALTERNATE_NO_ALTERNATE,    IO_PIN_LOCK_ENABLE},
   }


The configuration options of each pin are documented in [1]_. The naming conventions of foxbms for setting the pin's (alternate) function is found in ``io_cfg.h``
at ``IO_PIN_ALTERNATE_e``.
The clocks of the ports are enabled automatically when the pins are initialized through ``IO_Init(&io_cfg)`` as this functions calls ``IO_ClkInit(void)``.
In order to prevent other modules and functions to change the configuration of the pins the macro ``IO_PIN_LOCKING`` in ``io_cfg.h`` has to be defined. This macro 
automatically calls ``IO_LockPin(pin)`` for every pin which has defined ``IO_PIN_LOCK_ENABLE`` in ``io_cfg[]`` and locks the configuration registers of the corresponding pin.

Reading And Writing The Pins
----------------------------

- Reading the digital input of ``PIN_MCU_0_ISO_GPIO_IN_1``

.. code-block:: C

   ...
   IO_PIN_STATE_e pinstate;
   pinstate = IO_ReadPin(PIN_MCU_0_ISO_GPIO_IN_1);
   ...


- Writing the digital output of ``PIN_MCU_0_ISO_GPIO_OUT_0``

.. code-block:: C

   ...
   IO_PIN_STATE_e pinstate = IO_PIN_SET;
   IO_WritePin(PIN_MCU_0_ISO_GPIO_OUT_0, pinstate);
   ...

Related Modules
~~~~~~~~~~~~~~~
The contactor configuration is dependent on the |io module| configuration. The contactor configuration is found in the |contactor module|

  - src\\module\\config\\contactor_cfg.c
  - src\\module\\config\\contactor_cfg.h

---------------------------------------------------------------------------

Sources:

.. [1] Datasheet DM00071990 [PDF] http://www.st.com/st-web-ui/static/active/en/resource/technical/document/datasheet/DM00071990.pdf
