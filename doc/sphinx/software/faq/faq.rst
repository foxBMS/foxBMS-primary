.. _sw_faq:

Software FAQ
============

.. include:: ../../macros.rst

.. highlight:: C

This document contains how-tos for different kind of actions related to the software.

.. contents::
   :local:

How to create a task and change its priority and period?
--------------------------------------------------------

First, declare a new task configuration ``appltask_cfg.h`` file

.. code-block:: C

    /**
    * Task configuration of the 10ms application task
    */
    extern BMS_Task_Definition_s appl_tskdef_10ms;

The task configuration is a struct of type ``BMS_Task_Definition_s`` and defined as follows:

.. code-block:: C

    /**
    * struct for FreeRTOS task definition
    */
    typedef struct {
        uint32_t Phase;          /*!< (ms)                                 */
        uint32_t CycleTime;      /*!< (ms)                                 */
        OS_PRIORITY_e Priority;  /*!<                                      */
        uint32_t Stacksize;      /*!<  Defines the size, in words, of the 
                                       stack allocated to the idle task.   */
    } BMS_Task_Definition_s;

- Phase:     phase offset of the task in ms
- CycleTime: cycle time of the task in ms
- Stacksize: stack size allocated to the task
- Priority: task priority for scheduling

The task priorities are defined by CMSIS and consist of seven priorities:

.. code-block:: C

    typedef enum  {
      osPriorityIdle          = -3,          // /< priority: idle (lowest)
      osPriorityLow           = -2,          // /< priority: low
      osPriorityBelowNormal   = -1,          // /< priority: below normal
      osPriorityNormal        =  0,          // /< priority: normal (default)
      osPriorityAboveNormal   = +1,          // /< priority: above normal
      osPriorityHigh          = +2,          // /< priority: high
      osPriorityRealtime      = +3,          // /< priority: realtime (highest)
      osPriorityError         =  0x84        // /< system cannot determine priority
                                             //    or thread has illegal priority
    } osPriority;

The priorities ``osPriorityRealtime``, ``osPriorityHigh``, ``osPriorityAboveNormal`` and ``osPriorityNormal`` are already used by the |foxBMS| engine and therefore shouldn't be used. In conlusion the priorities ``osPriorityBelowNormal`` and ``osPriorityLow`` shall be used.

Second, add the task configuration in the ``appltask_cfg.c`` file

.. code-block:: C

    /**
     * predefined 10ms task for user code
     */
    BMS_Task_Definition_s appl_tskdef_10ms = { 0,   10, osPriorityBelowNormal, 512/4 };

Third, declare a task handle and a task function in the ´´appltask.c/h´´-file 

.. code-block:: C

    /**
     *  Definition of task handle 10 milliseconds task
     */
    xTaskHandle appl_handle_tsk_10ms;

   /**
     * @brief   10ms engine application task
     */
    extern void APPL_TSK_Cyclic_100ms(void);

The task initialization and creation is done in the function ``APPL_CreateTask()`` in the ``appltask.c`` file. Before assigning the task handle to the newly created task, a new thread needs to be defined for the operating system. This is done by a call of the function ``osThreadDef(name, thread, priority, instances, stacksz)``. The function parameters are

  - name: name of the function that represents the task
  - thread: os_pthread-pointer to the function that represents the task
  - priority: initial priority of the thread function
  - instances: number of possible thread instances(0 -> only one instance)
  - stacksize

The task handle is now, with a call of ``osThreadCreate``, assigned to the task.

.. code-block:: C

    // Cyclic Task 10ms
    osThreadDef(APPL_TSK_Cyclic_10ms, (os_pthread )APPL_TSK_Cyclic_10ms,
            appl_tskdef_10ms.Priority, 0, appl_tskdef_10ms.Stacksize);
    appl_handle_tsk_10ms = osThreadCreate(osThread(APPL_TSK_Cyclic_10ms), NULL);


The implementation of the task should be done like shown in the following:

.. code-block:: C

    void APPL_TSK_Cyclic_10ms(void) {
        while (os_boot != OS_SYSTEM_RUNNING)
        {
            ;
        }

        osDelayUntil(os_schedulerstarttime, appl_tskdef_10ms.Phase);

        while(1)
       { 
            uint32_t currentTime = osKernelSysTick();

            APPL_Cyclic_10ms();

            osDelayUntil(currentTime, appl_tskdef_10ms.CycleTime);
        }
    }

NOTE:
 - every task should have the while loop ``while(os_boot != OS_SYSTEM_RUNNING)`` at the beginning`of the funtion. This prevents that the task from being executed before |foxBMS| is completely initialized.
 - ``osDelayUntil(os_schedulerstarttime, appl_tskdef_10ms.Phase)`` sets the wished phase offset of the task.
 - tasks in FreeRTOS should never finish. Therefore, the actual task implementation is done in a ``while(1)``-loop
 - ``APPL_TSK_Cyclic_10ms`` serves as wrapper function for task implementation in ``APPL_Cyclic_10ms()``
 - the call of ``osDelayUntil(currentTime, appl_tskdef_10ms.CycleTime)`` sets the task in blocked state until the cycle time until the next period arrives
 - every task should be secured by the system monitoring module (``DIAG_SysMonNotify()``)
 - |foxBMS| provides two default tasks for user applications with a periods of 10ms and 100ms. 
 
.. code-block:: C

    void APPL_Cyclic_10ms(void) {
        DIAG_SysMonNotify(DIAG_SYSMON_APPL_CYCLIC_10ms, 0);        // task is running, state = ok

        /* User specific implementations:   */
        /*   ...                            */
        /*   ...                            */
    }


How to add a software module and take it into account with WAF
--------------------------------------------------------------

The steps to follow are:

 - Creation of a new subfolder (for example ``mymodule``) in one of the existing source folders ``application``, ``engine``, ``general``, ``module``, ``module/utils``)
 - Copy of all source files to the newly created subfolder 
 - Modification of the wscript file located in the chosen existing source folder, to add the new module. For example, to add a software module in the ``application`` folder, in the ``includes`` section of the wscript, the following new line has to be added:

.. code-block:: python

    os.path.join(bld.srcnode.abspath(), 'src', 'application', 'mymodule'), 
    
In case the new software module has to be used in another existing module, the same line has to be added in the wscript file corresponding to the existing module where it is imported.


How to change the multiplexer measurement sequence for the LTC driver?
----------------------------------------------------------------------

The sequence is defined in ``module/config/ltc_cfg.c``, via the array ``LTC_MUX_CH_CFG_t ltc_mux_seq_main_ch1[]``, which contains a concatenation of elements like

.. code-block:: C

   {
       .muxID    = 1,
       .muxCh    = 0xFF,
   },
   {
      .muxID    = 2,
      .muxCh    = 0,
   },
   {
      .muxID    = 2,
      .muxCh    = 1,
   },

There are 4 multiplexers on the foxBMS slaves, with IDs from 0 to 3. The multiplexers is chosen with the variable ``muxID``. Each multiplexer has 8 channels, chosen with the variable ``muxCh`` (between 0 and 7). Channel 0xFF means that the multiplxer is disabled, i.e., none of the 8 inputs is connected to the output.
With the sequence described above, multiplexer is disabled, then channel 0 of multiplexer 2 is read, then channel 1 of multiplexer 2 is read.

Typically, multiplexer 0 and 1 are used for temperature measurement, and multiplexer 2 and 3 are used for balancing monitoring (i.e., measure if a cell is being balanced or not). As a consequence, by default, measurements of multiplexer 0 and 1 are stored in a database structure of type ``DATA_BLOCK_CELLTEMPERATURE_s`` and measurement of multiplexer 2 and 3 are stored in a database structure of type ``DATA_BLOCK_BALANCING_FEEDBACK_s``.

For temperature measurement, the variable ``uint8_t ltc_muxsensortemperatur_cfg[6]`` contains the look-up table between temperature sensors and cells: the first entry defines the temperature sensor number assigned to the first cell, the second entry defines the temperature sensor number assigned to the second cell… It no look-up table is needed, this array should simply be filled with integers increasing from 0 to number of temperature sensors minus 1. In this example, ``muxsensortemperaturmain_cfg[6]`` has a size of 6 because it is the default number of temperature sensors on the foxBMS slaves. This must be adapted at two places:

 - In ``module/config/ltc_cfg.c``, where the variable is defined
 - In ``module/config/ltc_cfg.h``, in the declaration ``extern uint8_t ltc_muxsensortemperatur_cfg[6]``


.. _sw_faq_temperature_sensors:

How to change the relation between voltages read by multiplexer via LTC and temperatures?
-----------------------------------------------------------------------------------------

The function ``float LTC_Convert_MuxVoltages_to_Temperatures(float Vout)`` is defined in ``ltc.c``. It gets a voltage as input and returns a temperature. It can simply be changed to meet the application needs. 

How to configure the MCU clock
------------------------------

The configuration is defined in ``module/config/rcc_cfg.c`` via two structures:

.. code-block:: C

   RCC_OscInitTypeDef RCC_OscInitStruct = {
       .OscillatorType = RCC_OSCILLATORTYPE_HSE,
       .HSEState = RCC_HSE_ON,
       .PLL.PLLState = RCC_PLL_ON,
       .PLL.PLLSource = RCC_PLLSOURCE_HSE,
       .PLL.PLLM = RCC_PLL_M,    // Oscillator Clock: 8MHz -> (8Mhz / 8) * 336 / 2 -> 168Mhz
       .PLL.PLLN = RCC_PLL_N,
       .PLL.PLLP = RCC_PLL_P,
       .PLL.PLLQ = RCC_PLL_Q   // Oscillator Clock: 8MHz -> (8Mhz / 8) * 336 / 7   -> 48Mhz
   };

   RCC_ClkInitTypeDef RCC_ClkInitStruct = {
       .ClockType = RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2,
       .SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK,    // System Clock Source: PLL-Clock
                                                   // (Cortex-Core, AHB-Bus, DMA, memory)
       .AHBCLKDivider = RCC_AHBCLKDivider,         // Div=1 , AHB  CLOCK: 168MHz
       .APB1CLKDivider = RCC_APB1CLKDivider,       // Div=4 , APB1 CLOCK:  42MHz
       .APB2CLKDivider = RCC_APB2CLKDivider        // Div=2 , APB2 CLOCK:  84MHz
   };

:numref:`Fig. %s <systemclocks>` shows a summary of the system clocks, with the variables defined via the structures and their effect (either as divider or multiplier).

.. _systemclocks:
.. figure:: system_clocks.png
   :width: 100 %
   
   Clock system of the used ARM microcontroller

On the foxBMS master, a 8Mhz oscillator is used as clock source. It should be noted that some of the multipliers/dividers can take all integers values in a certain range, while others can only take a discrete set of values. The values must be defined so that the clock values are within the allowed ranges. These are defined in the microcontroller datasheet.

How to configure the LTC and SPI clocks?
----------------------------------------

In ``module/config/ltc_cfg.h``, the macro ``#define SPI_HANDLE_LTC &spi_devices[0]`` is defined. It points to a SPI handle: this SPI devices will be used for LTC communication. SPI handles are defined in ``module/config/spi_cfg.c``.
Depending on the SPI device chosen, the clock used will be peripheral clock 1 or 2 (this information is found in the MCU datasheet). The clock used by the SPI device is obtained after division of the peripheral clock frequency. In the SPI handle, the value for the divider is defined via
``.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128``
In the LTC driver, the SPI frequency is read directly via a HAL function and all timings are adapted automatically. The user must only ensure that the SPI frequency used for the LTC is not higher than 1MHz. This is the maximal frequency allowed for the LTC communication (as defined in the LTC datasheet).

How to configure the CAN clock?
-------------------------------

If the APB1 (Peripheral) clock changes, the CAN timing has to be adapted according to formula

.. math::

   \text{clock}_\text{CAN} = \frac{\text{APB1}}{(\text{prescaler} + \text{timequantums})}

   \text{timequantums} = 1 + \text{timequantumsBS1} + \text{timequantumsBS2}

The timequantums (TQ) are constrained to specific discrete values by the STM32 microcontroller. 

**Sample Configurations**

=============  ============   ============== =========== ===========
CAN clock      APB1           Prescaler      BS1         BS2
-------------  ------------   -------------- ----------- -----------
1.0 MHz        42 MHz         3              6 TQ        7 TQ
1.0 MHz        32 MHz         4              5 TQ        2 TQ
0.5 MHz        42 MHz         6              6 TQ        7 TQ
0.5 MHz        32 MHz         8              5 TQ        2 TQ
=============  ============   ============== =========== ===========
        
**Example:**

change the relevant low level driver handle in file 
/modules/config/can_cfg.c

``CAN_HandleTypeDef hcan1``
or 
``CAN_HandleTypeDef hcan2``

.. code-block:: C

   .Init.Prescaler = 3,        // CAN_CLOCK = APB1 = 42 MHz
                               // resulting CAN speed: APB1/prescaler/sumOfTimequants
                               // sum: 1tq for sync + BS1 + BS2
   .Init.BS1 = CAN_BS1_6TQ,    // --> CAN = 42 MHz/(3*14) = 1.0 MHz
   .Init.BS2 = CAN_BS2_7TQ,
   
.. math::

   \text{timequantums} = 1 + 6 + 7 = 14 
   
   \text{clock}_{CAN} = \frac{42.0\text{\ MHz}}{(3 \cdot  14)} = 1.0 \text{\ MHz}
   


Further details can be found in STM32 data sheet.

How to configure and drive I/O ports?
-------------------------------------

The pin configuration of the hardware is is defined in ``const IO_PIN_CFG_s io_cfg[]`` in file ``module/config/io_cfg.c``.
with entries like

.. code-block:: C

   {PIN_MCU_0_BMS_INTERFACE_SPI_MISO, IO_MODE_AF_PP, IO_PIN_NOPULL,
    IO_SPEED_HIGH, IO_ALTERNATE_AF5_SPI1, IO_PIN_LOCK_ENABLE}

The parameters are

 - Pin: Defines the pin name (defined in ``module/config/io_mcu0_cfg.h``).
 - Mode: The possibilities of the mode are defined in ``module/config io_cfg.c`` via the enum type ``IO_PIN_MODES_e``. Often used modes are ``IO_MODE_AF_PP`` for use of one alternate function of the pin, ``IO_MODE_INPUT`` to use the pin as an input, ``IO_MODE_OUTPUT_PP`` to use the pin as an output with push-pull functionality.
 - Pinpull: Defines wether the pin is used without pull-up or pull-down (``IO_PIN_NOPULL``), to use the pin with pull-up (``IO_PIN_PULLUP``) or to use the pin with pull-down (``IO_PIN_PULLDOWN``).
 - Speed: Defines the speed of the pin (``IO_SPEED_LOW``, ``IO_SPEED_MEDIUM``, ``IO_SPEED_FAST`` or ``IO_SPEED_HIGH``).
 - Alternate: Defines if the signal/pin uses an alternate function or not. If no alternate function is used this is set to ``IO_ALTERNATE_NO_ALTERNATE``. If the signal/pin uses a alternate function one can choose from the possibilities from the enumeration ``IO_PIN_ALTERNATE_e`` in ``module/config/io_cfg.h``.
 - Pinlock: ``IO_PIN_LOCK_DISABLE`` or ``IO_PIN_LOCK_ENABLE`` to disable or enable pin locking.
 - Initvalue: Sets the initial state of the pin in case of an output pin; The pin is set to ``IO_PIN_RESET`` for 0/low and to ``IO_PIN_SET`` for 1/high. If no value is given for a output pin it is set to low.
 
As explained above, the signal names are to be defined in ``module/config/io_mcu0_cfg.h`` with macros like

.. code-block:: C

   #define PIN_MCU_0_BMS_INTERFACE_SPI_MISO IO_PA_6

where ``IO_Px_y`` corresponds to the physical pin on the MCU, with ``x`` the port (A,B,C… depending on the MCU used) and ``y`` the pin number on the port (0,1,2… depending on the MUC used).

This configuration is initialized in ``main.c`` with the function call ``IO_Init(&io_cfg[0])``.

Pins configured as output are driven with the function with ``IO_PIN_RESET`` or ``IO_PIN_SET`` to set the pin to low or high. Example (The signal/pin name corresponds to the one defined in ``module/config/io_mcu0_cfg.h``).:

.. code-block:: C

   IO_WritePin(PIN_MCU_0_BMS_INTERFACE_SPI_MISO, IO_PIN_RESET);     // set pin low
   IO_WritePin(PIN_MCU_0_TO_MCU_1_INTERFACE_SPI_MISO, IO_PIN_SET);  // set pin high

The states of pins configured as input are read with ``IO_ReadPin(<Signalname>)``-function. The function returns ``IO_PIN_RESET`` or ``IO_PIN_SET`` (0 or 1). Example (The signal/pin name corresponds to the one defined in ``module/config/io_mcu0_cfg.h``).:

.. code-block:: C

   IO_PIN_STATE_e pinstate = IO_PIN_RESET;
   pinstate = IO_ReadPin(PIN_MCU_0_BMS_INTERFACE_SPI_NSS);

How to add and configure interrupts?
------------------------------------

The interrupt configuration can be found in ``general/config/nvic_cfg.c``, via the variable ``NVIC_InitStruct_s nvic_interrupts[]`` which contains entries of the form
``{ DMA2_Stream2_IRQn, 2, NVIC_IRQ_LOCK_ENABLE, NVIC_IRQ_ENABLE }``

The configuration parameters are:
 - Symbolic name of interrupt source (as defined in the system file stm32f429xx.h)
 - Interrupt priority: number between 0 and 15, a lower number means a higher priority
 - Parameter irqlock: if set to ``NVIC_IRQ_LOCK_ENABLE``, the interrupt is locked according to the initial state and can not be modified by the interface functions ``NVIC_EnableInterrupts()`` or ``NVIC_DisableInterrupts()``
 - Initial state of interrupt source: set to ``NVIC_IRQ_ENABLE`` to get the interrupt enabled by the initialization function. In case of ``NVIC_IRQ_DISABLE``, the interrupt must be activated by calling ``NVIC_EnableInterrupts()`` after the initialization  
 
In ``general/config/stm32f4xx_it.c``, a corresponding callback function must be defined (for example
``void DMA2_Stream2_IRQHandler(void)`` for DMA stream 2 of DMA device 2. It will be called when the interrupt is triggered.

For a proper operation, the interrupt handling (callback function) has to execute the following steps:

 - Clear the pending interrupt with for example ``HAL_NVIC_ClearPendingIRQ(DMA2_Stream2_IRQn)``
 - Call a custom or the HAL IRQ handler with for example ``HAL_DMA_IRQHandler(&dma_devices[0])``
   Note:
   Interrupt routines with interrupt priority above the maximum FreeRTOS configuration level (``configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY``) must not call FreeRTOS API functions. This interrupts are real-time interrupts which are bypassing the operating system.
   Interrupt routines with interrupt priority equal or lower of this maximum level must call the corresponding FreeRTOS API functions with ending ``...._FROM_ISR()`` 
 
Check the documentation (datasheet, reference manual) of the interrupt source for additional steps.

How to add a database entry and to read/write it?
-------------------------------------------------

The example of the entry for cell voltages is taken. In ``engine/config/database_cfg.h``, the definition of a block must be added
``#define DATA_BLOCK_ID_CELLVOLTAGE DATA_BLOCK_1``
with one available block. The blocks are defined via the following enumeration:

.. code-block:: C

   typedef enum {
       DATA_BLOCK_1        = 0,
       DATA_BLOCK_2        = 1,
       DATA_BLOCK_3        = 2,
       DATA_BLOCK_4        = 3,
       DATA_BLOCK_5        = 4,
       DATA_BLOCK_6        = 5,
       DATA_BLOCK_7        = 6,
       DATA_BLOCK_8        = 7,
       DATA_BLOCK_9        = 8,
       DATA_BLOCK_10       = 9,
       DATA_BLOCK_11       = 10,
       DATA_BLOCK_MAX      = DATA_MAX_BLOCK_NR,
   } DATA_BLOCK_ID_TYPE_e;


If more than ``DATA_BLOCK_MAX`` blocks are needed, it must be changed in the defines

.. code-block:: C

   #define DATA_MAX_BLOCK_NR                11
   #define DATA_MAX_USED_BLOCK_NR           11

A structure must then be declared for the block ID created:

.. code-block:: C

   /*  data structure declaration of DATA_BLOCK_ID_CELLVOLTAGE */
   typedef struct {
       uint16_t      voltage[NR_OF_BAT_CELLS];    //unit: mV
       uint32_t      previous_timestamp;
       uint32_t      timestamp;
       uint8_t       state;
   } DATA_BLOCK_CELLVOLTAGE_s;


This structure can contain all the data needed for the entry.
Then in ``engine/config/database_cfg.c``, a variable with the structure type must be declared
``DATA_BLOCK_CELLVOLTAGE_s data_block_cellvoltage[DOUBLE_BUFFERING]``.
The user can choose ``SINGLE_BUFFERING`` or ``DOUBLE_BUFFERING``.
The last step is to add an entry in the structure ``DATA_BASE_HEADER_s  data_base_header[]``:

.. code-block:: C

   {
   (void*)(&data_block_cellvoltage[0]),
   sizeof(DATA_BLOCK_CELLVOLTAGE_s),
   DOUBLE_BUFFERING,
   },


With either ``SINGLE_BUFFERING`` or ``DOUBLE_BUFFERING`` (the same as in the structure declaration).
When access to the created database entry is needed, a local variable with the corresponding type must be created in the module where it is needed:
``DATA_BLOCK_CELLVOLTAGE_s cellvoltage;``

Access to a data field is made with the usual C-syntax: ``cellvoltage.voltage[i]``

Getting the data from the database in the local variable is made via:
``DATA_GetTable(&cellvoltage,DATA_BLOCK_ID_CELLVOLTAGE)``

Storing data from the local variable to the database is made via:
``DATA_StoreDataBlock(&cellvoltage,DATA_BLOCK_ID_CELLVOLTAGE)``

How to store data in the backup SRAM
------------------------------------

The STM32F4 has 4kB Backup SRAM. Variables can be stored there with the keywork ``BKP_SRAM`` which is defined in ``src/general/config/gobal.h``.

Example:

.. code-block:: C

   #define DIAG_FAIL_ENTRY_LENGTH (50)
   DIAG_ERROR_ENTRY_s BKP_SRAM diag_memory[DIAG_FAIL_ENTRY_LENGTH];

.. _faq_can_manually_add:

How to manually add CAN entries (transmit and receive) and to change the transmit time period?
----------------------------------------------------------------------------------------------

Several steps have to be done to add a transmit message and signal 
(message and signal in receive direction in parentheses):

**File: module/config/cansignal_cfg.h**

1. The message name must be added. For TX, i.e., transmit data, 
   a message entry must be added in the enumeration ``CANS_messagesTx_e``.
   (RX, i.e., receive, ``CANS_messagesRx_e``).
 
2. The macro ``#define NR_MESSAGES_TX`` (``#define NR_MESSAGES_RX``) must be incremented accordingly. 

3. Then one or more signal names must be added in the enumeration ``CANS_signalsTx_e`` (``CANS_signalsRx_e``).

4. The macro ``NR_SIGNALS_TX``  must be incremented accordingly (``NR_SIGNALS_RX``)

**File: module/config/cansignal_cfg.c**

5. The next step is to implement the message.

   For TX data, the message must be added in array ``const CANS_message_t cans_messages_tx[NR_MESSAGES_TX]``
   (``const CANS_message_t cans_messages_rx[NR_MESSAGES_RX]``).
 
   The message looks like ``{ 0x550, 8, 100, 20, NULL_PTR }``

   The parameters are:

   - Message ID (11bit standard identifier used in foxBMS)
   - Data Length Code (i.e., number of bytes), usually 8
   - Transmit period in ms, must be multiple of ``CANS_TICK_MS`` (irrelevant for RX messages)
   - Delay for sending message the first time in ms, must be multiple of ``CANS_TICK_MS`` (irrelevant for  RX messages)
   - a function pointer to be called after transmission (reception), usually NULL_PTR to do nothing

6. Then the signals added in the header must be added in the .c file, too, in
``const CANS_signal_s cans_signals_tx[NR_SIGNALS_TX]`` (``const CANS_signal_s cans_signals_rx[NR_SIGNALS_RX]``). 

  A signal looks like

  .. code-block:: C
  
    {CANS_MSG_BMS2}, 0, 4, 15, 0, 1, 0, FALSE , FALSE, FALSE, {CANS_SIGNAL_NONE}, &cans_setmux, &cans_getupdatemux
  
  The parameters are:

   - Symbolic name of the message containing the signal (defined in cans_messagesTx_e, in the header file)
   - Start bit of signal
   - Signal length in bits
   - maximum value (float), not relevant in SW at the time
   - minimum value (float), not relevant in SW at the time
   - scaling factor (float), not relevant in SW at the time
   - scaling offset (float), not relevant in SW at the time
   - flag multiplexed (boolean, use FALSE)
   - flag multiplexor (boolean, use FALSE)
   - number of multiplex value (int)
   - symbolic name of multiplexor signal, if not multiplexed use {CANS_SIGNAL_NONE}
   - Callback function for setter (when CAN msg is received) (NULL_PTR if no function needed, e.g., for transmit)
   - Callback function gor getter (when CAN msg is transmitted) (NULL_PTR if no function needed, e.g., for receive only) 
 
  The callback functions must be declared and implemented in cansignal_cfg.c.

  The multiplexed signal management is a little bit more complex and therefore 
  described in detail in CAN Signal module :ref:`CANMultiplex`.

  For special cases a getter and a setter function pointer can be necessary (e.g. a multiplexor signal), but usually
  
    * setter function pointer directs to function where received signal is stored and 
    * getter function pointer directs to function where signal for transmission is returned

.. _faq_can_program_add:

How to programmaticaly generate CAN entries?
-------------------------------------------- 

In future releases of foxBMS the automatic code generation for CAN signal and message definition will be used.

This is done with a set of python scripts and not mature for production usage.

Here is a simple introduction of the actual procedure to generate code for CAN related modules:

1. adapt the CAN database file in ``.\tools\genCAN\config\BMS.dbc`` to your needs

2. call genCAN.py in tools ``python tools\genCAN.py -i tools\genCAN\config\BMS.dbc``

3. compare and merge the generated files to the respective source and documentation files

  The following files are generated per default:

    * cansignal_cfg.h
    * cansignal_cfg.c
    * canmatrix.rst
    * can_cfg.c

How to change the blink period of the indicator LEDs?
-----------------------------------------------------

It is defined in via the three variables

 - ``led_0_nbr_of_blink``
 - ``led_1_nbr_of_blink``
 - ``led_blink_time``
 
in ``module/utils/led.c``. Currently the three variables are set directly in the function ``LED_Ctrl()``. If desired or needed, the three values can be read from the database and so set elsewhere in the code.

The time is set via ``led_blink_time``, which is given in ms. The LED cycle is divided in two periods, T0 and T1. Each of both periods lasts led_blink_time ms. During T0, LED0 is on or blinks and LED1 is off. During T1, LED0 is off and LED1 is on or blinks.
The behavior described as ''is on or blinks'' is defined via ``led_0_nbr_of_blink`` for LED0 and via ``led_1_nbr_of_blink`` for LED1.
If ``led_0_nbr_of_blink`` is set to 1, LED0 is on during T0. If ``led_0_nbr_of_blink`` is set to n greater than 1, LED0 will blink n times during T1. For example, if the period is set to 3000ms and ``led_0_nbr_of_blink`` set to 3, LED0 will have the following behavior during T0:

 - On for 500ms
 - Off for 500ms
 - On for 500ms
 - Off for 500ms
 - On for 500ms
 - Off for 500ms
 
The behavior is the same for LED1 with ``led_1_nbr_of_blink``.
With this functionality, each LED can blink independently n times. I each LED can blink up to four times, the LED can visually give 16 different messages.

The adjustment of the blink period is provided to make reading by the user easier: for higher number of blinks, the blink period can be make longer (for example, it is easier to count 4 blinks in 3 s than 4 blinks in 1 s).

How to add an entry for the diag module?
----------------------------------------


Adding a new sensitivy level is done in ``src/engine/config/diag_cfg.h`` by adding a new ``#define`` in

.. code-block:: C

   #define DIAG_ERROR_SENSITIVITY_HIGH        (0)   // logging at first event
   #define DIAG_ERROR_SENSITIVITY_MID         (5)   // logging at fifth event
   #define DIAG_ERROR_SENSITIVITY_LOW         (10)  // logging at tenth event
   #define DIAG_ERROR_SENSITIVITY_CUSTOM      (100) // logging at 100th event
  
and errors can defined

.. code-block:: C

   #define     DIAG_CH_CUSTOM_FAILURE             DIAG_ID_XX

and the error is then added in ``DIAG_CH_CFG_s  diag_ch_cfg[]`` in file ``src/engine/config/diag_cfg.c``

.. code-block:: C

   DIAG_CH_CFG_s  diag_ch_cfg[] = {
    {DIAG_CH_CUSTOM_FAILURE,        DIAG_GENERAL_TYPE, DIAG_ERROR_SENSITIVITY_HIGH,
     DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},


The number of logged error events can be changed by chaging the value of the macro

.. code-block:: C

   #define DIAG_FAIL_ENTRY_LENGTH               (50)    // Number of errors that can be logged

**Imporant Note**

The diag module is a powerfull module for general error handling. The user has to be aware of 
timing when using custom diag entries. As example how to use this module correct syscontrol is choosen.

 - The function ``SYSCTRL_Trigger()`` is called in the 10 milliseconds task (``ENG_TSK_Cyclic_10ms()``), meaning every 10  
   milliseconds this function must be executed.
 - In the diagnosis-module header ``diag_cfg.h`` there is the enum ``DIAG_SYSMON_MODULE_ID_e`` for 
   the different error types that are handeled by the diagnosismodule. For syscontrol errors there is ``DIAG_SYSMON_SYSCTRL_ID``.
 - In the diagnosis-module source ``diag_cfg.c`` there is the ``diag_sysmon_ch_cfg[]`` array assigning timings to this error, in this
   case 20 milliseconds.
   
.. code-block:: C

   {DIAG_SYSMON_SYSCTRL_ID, DIAG_SYSMON_CYCLICTASK, 20,
    DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu2},

This means every time ``SYSCTRL_Trigger()`` is called, the function to indicating "syscontrol is running" has to be exectued. If this is not done,
the diagnosis module will set the syscontrol to the error state.
Therefore the user must set up functions, which he wants to be supervised by the diagnosis module, that they are still running, in this way:

.. code-block:: C

   void SYSCTRL_Trigger(SYSCTRL_TRIG_EVENT_e event) {
       DIAG_SysMonNotify(DIAG_SYSMON_SYSCTRL_ID, 0);  // task is running, therefore
                                                      // reset state to 0
       /* user code */
   }


How to use contactors without feedback?
---------------------------------------

In the file ``src/module/config/contactor_cfg.c``, the contactors are defined with

.. code-block:: C

   CONT_CFG_s cont_contactors_cfg[NR_OF_CONTACTORS] = {
           {CONT_PLUS_MAIN_CONTROL,        CONT_PLUS_MAIN_FEEDBACK,        CONT_FEEDBACK_NORMALLY_OPEN},
           {CONT_PLUS_PRECHARGE_CONTROL,   CONT_PLUS_PRECHARGE_FEEDBACK,   CONT_FEEDBACK_NORMALLY_OPEN},
           {CONT_MINUS_MAIN_CONTROL,       CONT_MINUS_MAIN_FEEDBACK,       CONT_FEEDBACK_NORMALLY_OPEN}
   };

The following change must be made to configure a contactor without feedback (in this case, the precharge contactor is taken as example): 

.. code-block:: C

   {CONT_PLUS_PRECHARGE_CONTROL,   CONT_HAS_NO_FEEDBACK,   CONT_FEEDBACK_TYPE_DONT_CARE}

With this configuration, the feedback value will always be equal to the expected value for the precharge contactor.

.. _can_debug_message:

How to simply trigger events via CAN ?
--------------------------------------

The CAN message with ID 0x55E is considered as a "debug message". If received, the function ``cans_setdebug()`` defined in ``cansignal_cfc.c`` is called.
There, a switch case is used on the first byte (byte0) of the message to define what to do. This means that 256 actions are possible, and that 7 bytes remain to transfer data
in the debug message. 


How to start/stop balancing the battery cells?
----------------------------------------------

Currently, this is done by sending the debug message with the first byte (byte0) equal to 14 (0x0E). If the second byte is set to 1, balancing will start. If set to 0, balancing stops.
The third byte corresponds to the threshold for balancing. The minimum voltage is determined at pack level, and every cell whose voltage is greater than
minimum+threshold is balanced.


How to set SOC via CAN?
-----------------------

This is done via the debug message with the first byte (byte0) equal to 11 (0x0B). The SOC is defined via the two next bytes, also with 16 bit (byte1 byte2).
The SOC is given in 0.01% unit, which means that the 16 bit number should be comprised between 0 and 10000. If a smaller value is given, the SOC will be set to 0%.
If a greater value is given, SOC will be set to 100%. 

How to add/remove temperature sensors?
--------------------------------------

To remove one temperature sensor, the first step is to change

.. code-block:: C

    #define NR_OF_TEMP_SENSORS_PER_MODULE           6

to

.. code-block:: C

    #define NR_OF_TEMP_SENSORS_PER_MODULE           7

in global.h.

However, this obvious step is not enough.

In ltc_cfg.c, the variable ``LTC_MUX_CH_CFG_s ltc_mux_seq_main_ch1[]`` which indicates the channel sequence to read on the multiplexer
has to be modified. One sensor channel on one multiplexer has to be added. In this example, on multiplexer with ID 0, channel 6 is read additionally.

Concretely,

.. code-block:: C

    {
        .muxID    = 1,
        .muxCh    = 0xFF,
    },
    {
        .muxID    = 2,
        .muxCh    = 0xFF,
    },
    {
        .muxID    = 3,
        .muxCh    = 0xFF,
    },
    {
        .muxID    = 0,
        .muxCh    = 0,
    },
    {
        .muxID    = 0,
        .muxCh    = 1,
    },
    {
        .muxID    = 0,
        .muxCh    = 2,
    },
    {
        .muxID    = 0,
        .muxCh    = 3,
    },
    {
        .muxID    = 0,
        .muxCh    = 4,
    },
    {
        .muxID    = 0,
        .muxCh    = 5,
    },
    {
        .muxID    = 0,             //configure the multiplexer to be used
        .muxCh    = 6,            //configure input to ne used on the selected multiplexer
    },

must be used instead of

.. code-block:: C

    {
        .muxID    = 1,
        .muxCh    = 0xFF,
    },
    {
        .muxID    = 2,
        .muxCh    = 0xFF,
    },
    {
        .muxID    = 3,
        .muxCh    = 0xFF,
    },
    {
        .muxID    = 0,
        .muxCh    = 0,
    },
    {
        .muxID    = 0,
        .muxCh    = 1,
    },
    {
        .muxID    = 0,
        .muxCh    = 2,
    },
    {
        .muxID    = 0,
        .muxCh    = 3,
    },
    {
        .muxID    = 0,
        .muxCh    = 4,
    },
    {
        .muxID    = 0,
        .muxCh    = 5,
    },

Then the look-up table must be changed: it allows defining the correspondence between multiplexer channel and sensor order in the temperature array.

By default, the mapping keeps the same order.

In our example,

.. code-block:: C

    const uint8_t ltc_muxsensortemperatur_cfg[6] = {
        1-1 ,       /*!< index 0 = mux 0, ch 0 */
        2-1 ,       /*!< index 1 = mux 0, ch 1 */
        3-1 ,       /*!< index 2 = mux 0, ch 2 */
        4-1 ,       /*!< index 3 = mux 0, ch 3 */
        5-1 ,       /*!< index 4 = mux 0, ch 4 */
        6-1 ,       /*!< index 5 = mux 0, ch 5 */
        //7-1 ,     /*!< index 6 = mux 0, ch 6 */
        //8-1 ,     /*!< index 7 = mux 0, ch 7 */
        //9-1 ,     /*!< index 8 = mux 1, ch 0 */
        //10-1 ,    /*!< index 9 = mux 1, ch 1 */
        //11-1 ,    /*!< index 10 = mux 1, ch 2 */
        //12-1 ,    /*!< index 11 = mux 1, ch 3 */
        //13-1 ,    /*!< index 12 = mux 1, ch 4 */
        //14-1 ,    /*!< index 13 = mux 1, ch 5 */
        //15-1 ,    /*!< index 14 = mux 1, ch 6 */
        //16-1      /*!< index 15 = mux 1, ch 7 */
    };

must be changed to

.. code-block:: C

    const uint8_t ltc_muxsensortemperatur_cfg[7] = {
        1-1 ,       /*!< index 0 = mux 0, ch 0 */
        2-1 ,       /*!< index 1 = mux 0, ch 1 */
        3-1 ,       /*!< index 2 = mux 0, ch 2 */
        4-1 ,       /*!< index 3 = mux 0, ch 3 */
        5-1 ,       /*!< index 4 = mux 0, ch 4 */
        6-1 ,       /*!< index 5 = mux 0, ch 5 */
        7-1 ,     /*!< index 6 = mux 0, ch 6 */
        //8-1 ,     /*!< index 7 = mux 0, ch 7 */
        //9-1 ,     /*!< index 8 = mux 1, ch 0 */
        //10-1 ,    /*!< index 9 = mux 1, ch 1 */
        //11-1 ,    /*!< index 10 = mux 1, ch 2 */
        //12-1 ,    /*!< index 11 = mux 1, ch 3 */
        //13-1 ,    /*!< index 12 = mux 1, ch 4 */
        //14-1 ,    /*!< index 13 = mux 1, ch 5 */
        //15-1 ,    /*!< index 14 = mux 1, ch 6 */
        //16-1      /*!< index 15 = mux 1, ch 7 */
    };

A last step has to be done in ltc_cfg.h: adjust the size of the array, by changing

.. code-block:: C

    extern const uint8_t ltc_muxsensortemperatur_cfg[6];

to

.. code-block:: C

    extern const uint8_t ltc_muxsensortemperatur_cfg[7];

How to change the resolution of the temperature values stored?
--------------------------------------------------------------

The scaling of the raw values read from the LTC6804 chip is made in ltc.c, in the function "static void LTC_SaveTemperatures_SaveBalancingFeedback(void)".

First, the raw data are read from the SPI buffer with

.. code-block:: C

    val_ui=*((uint16_t *)(&LTC_MultiplexerVoltages[2*((LTC_NUMBER_OF_LTC_PER_MODULE*i*LTC_N_MUX_CHANNELS_PER_LTC)+muxseqptr->muxID*LTC_N_MUX_CHANNELS_PER_MUX+muxseqptr->muxCh)]));

Then the raw data are scaled and are available in Volt.

.. code-block:: C

    val_fl = ((float)(val_ui))*100e-6;

Then the read voltage is converted into a temperature with the a look-up table

.. code-block:: C

    val_si = (int16_t)(LTC_Convert_MuxVoltages_to_Temperatures(val_fl));

The last step is to store the temperature into a ``sint16`` variable.

.. code-block:: C

    ltc_celltemperature.temperature[i*(NR_OF_TEMP_SENSORS_PER_MODULE)+sensor_idx]=val_si;

At this step, the resolution can be changed. For instance, m°C could be stored instead of °C. 

