.. include:: ../../../macros.rst


==
IO
==

.. highlight:: C

This section describes the configuration of the pins of the |foxbms| microcontrollers on the |BMS-Master| using the |mod_io|. The |mod_io| allows an easy way to configure all pins of the microcontroller at a central position, reading the input signals and writing signals to the output pins of the microcontroller.



Module Files
~~~~~~~~~~~~

Driver:
 - ``src\\module\\io\\io.c``
 - ``src\\module\\io\\io.h``
 
Driver Configuration:
 - ``src\\module\\config\\io_cfg.c``
 - ``src\\module\\config\\io_cfg.h``
 - ``src\\module\\config\\io_mcu0.h``
 - ``src\\module\\config\\io_package_cfg.h``


Configuration of the GPIOs
~~~~~~~~~~~~~~~~~~~~~~~~~~

Initialization of the GPIOs
---------------------------

The following example shows how to configure the pins of the microcontrollers. The starting point is the following pin configuration:

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

To define a signal, a ``#define`` has to be set in ``io_cfg_foxbms.h`` describing the signal:

.. code-block:: C
   
   ...
   #define PIN_MCU_0_FMC_RAM_A0               IO_PF_0
   #define PIN_MCU_0_FMC_RAM_A1               IO_PF_1
   ...
   #define PIN_MCU_0_ISO_GPIO_IN_1            IO_PD_13
   #define PIN_MCU_0_ISO_GPIO_OUT_0           IO_PG_9
   ...

The initialization of this configuration on the hardware is executed through the |mod_io| function ``IO_Init(*io_cfg)``. The ``const IO_PIN_CFG_s io_cfg[]`` has to be configured in ``io_cfg.h``. In the given example, this looks like:

.. code-block:: C
   
   IO_PIN_CFG_s io_cfg[] = {
   {PIN_MCU_0_FMC_RAM_A0,      IO_MODE_AF_PP,       IO_PIN_NOPULL,    IO_SPEED_HIGH,    IO_ALTERNATE_AF12_FMC,        IO_PIN_LOCK_ENABLE},
   {PIN_MCU_0_FMC_RAM_A1,      IO_MODE_AF_PP,       IO_PIN_NOPULL,    IO_SPEED_HIGH,    IO_ALTERNATE_AF12_FMC,        IO_PIN_LOCK_ENABLE},
   ...
   {PIN_MCU_0_ISO_GPIO_IN_1,   IO_MODE_INPUT,       IO_PIN_PULLDOWN,  IO_SPEED_FAST,    IO_ALTERNATE_NO_ALTERNATE,    IO_PIN_LOCK_ENABLE},
   {PIN_MCU_0_ISO_GPIO_OUT_0,  IO_MODE_OUTPUT_PP,   0,                0,                IO_ALTERNATE_NO_ALTERNATE,    IO_PIN_LOCK_ENABLE},
   }


The configuration options of each pin are documented in [1]_. The naming conventions of |foxbms| for setting the pin (alternate) function is found in ``io_cfg.h`` at ``IO_PIN_ALTERNATE_e``. The clocks of the ports are enabled automatically when the pins are initialized through ``IO_Init(&io_cfg)`` as this function calls ``IO_ClkInit(void)``. In order to prevent other modules and functions to change the configuration of the pins, the macro ``IO_PIN_LOCKING`` in ``io_cfg.h`` has to be defined. This macro automatically calls ``IO_LockPin(pin)`` for every pin which has defined ``IO_PIN_LOCK_ENABLE`` in ``io_cfg[]`` and locks the configuration registers of the corresponding pin.

Reading and Writing the Pins
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
The contactor configuration is dependent on the |mod_io| configuration. The contactor configuration is found in the |mod_contactor|:

  - ``src\\module\\config\\contactor_cfg.c``
  - ``src\\module\\config\\contactor_cfg.h``

---------------------------------------------------------------------------

Sources:

.. [1] Datasheet DM00071990 [PDF] http://www.st.com/st-web-ui/static/active/en/resource/technical/document/datasheet/DM00071990.pdf
