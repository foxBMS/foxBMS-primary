LTC
===

.. include:: ../../../macros.rst

.. highlight:: C

The driver communicates with the LTC monitoring chips in daisy-chain configuration. The chips are used to

 - measure cell voltages,
 - measure voltage on GPIOs directly
 - measure voltage on up to 32 inputs via I2C-driven multiplexer
 - enable balancing on cells.

.. ignore .. sectnum::

.. contents:: Table Of Contents

.. highlight:: C

Module Files
~~~~~~~~~~~~

Driver:
 - src\\module\\ltc\\ltc.c
 - src\\module\\ltc\\ltc.h
 
Driver Configuration:
 - src\\module\\config\\ltc_cfg.c
 - src\\module\\config\\ltc_cfg.h


Dependencies
~~~~~~~~~~~~

 - src\\module\\cpu\\cpu.h
 - src\\module\\diag\\diag.h
 - src\\module\\spi\\spi.h


Detailed Description of LTC Module
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

State Machine
-------------

The driver is organized in a state machine, which is mainly implemented with 4 functions:

 - ``LTC_Trigger()``: called every millisecond, contains the sequence of events in the state machine
 - ``LTC_Ctrl()``: makes state requests to the state machine, so that the LTC chip performs its tasks
 - ``LTC_SetStateRequest()``: used in ``LTC_Ctrl()`` to make the state requests
 - ``LTC_CheckStateRequest()``: called by ``LTC_SetStateRequest()`` to check the state requests. Returns the result of the check immediately.

The state machine functions as follows.
First, the LTC daisy chained is initialized as shown in :numref:`fig. %s <ltc_figure1>`. 

.. _ltc_figure1:
.. figure:: LTC_Statemachine1.png
   :width: 100 %

   LTC state machine; initialization

After initilization, the state machine goes in the ``idle``-state, where the user can make five different requests:

 - measure voltages
 - read measured voltages
 - read multiplexer inputs
 - read GPIO inputs
 - balance cells

The function ``LTC_SetStateRequest()`` is used to make these state requests to the LTC state machine. :numref:`Fig. %s <ltc_figure2>` and :numref:`%s <ltc_figure3>` detail how these possible state requests are handled.
 
.. _ltc_figure2:
.. figure:: LTC_Statemachine2.png
   :width: 100 %
   
   LTC state machine; voltage measurement, voltage readout, all GPIOs measurement

.. _ltc_figure3:
.. figure:: LTC_Statemachine3.png
   :width: 100 %

   LTC state machine; measurement of a single GPIO 

Results Retrieval and Balancing Orders
--------------------------------------

The results of the measurements are written in the database. The balancing is made according to the control variable set in the database.
The corresponding variables are:

.. code-block:: C

   DATA_BLOCK_CELLVOLTAGE_s           ltc_cellvoltage; //cell voltages
   DATA_BLOCK_CELLTEMPERATURE_s       ltc_celltemperature; //cell temperature
   DATA_BLOCK_MINMAX_s                ltc_minmax; //minimum and maximum values at battery pack
                                                  //level for voltages and temperatures
   DATA_BLOCK_BALANCING_FEEDBACK_s    ltc_balancing_feedback; //result from balancing feedback
                                                              //(is each cell balanced or not)
   DATA_BLOCK_BALANCING_CONTROL_s     ltc_balancing_control; //balancing orders read from database



LTC Configuration
~~~~~~~~~~~~~~~~~

SPI Interface
-------------

In ``ltc_cfg.h``, the SPI devices used by the LTC is defined by the macro ``SPI_HANDLE_LTC``.
The spi handle must be chosen from the list ``SPI_HandleTypeDef spi_devices[]`` in ``spi_cfg.c``. The frequency of the used SPI must be adjusted with the configuration
in ``spi_cfg.c``, so that the frequency is not higher than 1MHz. This is the maximum frequency allowed for the LTC chip.
The function ``LTC_SetTransferTimes()`` sets the waiting times used for the LTC driver automatically at startup. It uses ``LTC_GetSPIClock()``
to get the SPI clock frequency automatically.


Measurement Mode and Channel Selection
--------------------------------------

When a request is made with the function ``LTC_SetStateRequest()`` to measure cell voltages or multiplexer inputs, two arguments must be given.
The first one is the measurement mode, which can be:
 
 - normal
 - filtered
 - fast
 
These modes are defined in the LTC chip data sheet [ltc_datasheet]_.
For cell voltage measurements, the macro ``VOLTAGE_MEASUREMENT_MODE`` is defined in ltc_cfg.h. For multiplexers measurement,
the macro ``GPIO_MEASUREMENT_MODE`` is defined in ``ltc_cfg.h``.

The second argument is the number of channels: The following enums are defined in ltc.h:
 
 - ``LTC_ADCMEAS_ALLCHANNEL`` to measure all cell voltages/to measure all 5 GPIO inputs
 - ``LTC_ADCMEAS_SINGLECHANNEL`` to measure 2 cell voltages/to measure 1 GPIO input

Multiplexer Sequence
--------------------

In case of single GPIO measurement, the multiplexer sequence to be read is defined in ``ltc_cfg.c`` with the
variable ``LTC_MUX_CH_CFG_t ltc_mux_seq_main_ch1[]``. It is a list of elements of the form

.. code-block:: C

   {
       .muxID    = 1,
       .muxCh    = 3,
   }
    
where the multiplexer to select (muxID, can be 0, 1, 2 or 3) and the channel to select (muxCh, from 0 to 7) are defined. Channel 0xFF means
that the multiplxer is turned of. This is used to avoid cross-talk between multiplexers. 

Temperature Sensor Assignment
-----------------------------

For temperature sensors, the variable

.. code-block:: C

   uint8 ltc_muxsensortemperaturmain_cfg[8] =
   {    8-1 ,  //channel 0
       7-1 ,  //channel 1
       6-1 ,  //channel 2
       5-1 ,  //channel 3
       4-1 ,  //channel 4
       3-1 ,  //channel 5
       2-1 ,  //channel 6
       1-1   //channel 7
   }

is used gives the correspondence between the channel measured and the index used. In the above example, channel 0 of the multiplexer corresponds to
temperature sensor 8. If muxseqptr is the multiplexer sequence of
type ``LTC_MUX_CH_CFG_t`` as defined above, the sensor index is retrieved in the variable ``sensor_idx`` via:

.. code-block:: C

   sensor_idx = ltc_muxsensortemperaturmain_cfg[muxseqptr->muxCh];

Usage
~~~~~

The LTC cycle (cell voltage measurement, GPIO measurement, balancing control) is implemented in the function ``LTC_Ctrl()``. The default configuration
is a measurement with multiplexers. Once the LTC daisy chain has been initialized, the cycle is as follow:

 - measure cell voltages
 - measure NUMBER_OF_MUX_MEASUREMENTS_PER_CYCLE multiplexer inputs
 - measure cell voltage
 - repeat till all multiplexer inputs have been read
 - balance cells according to the data in database


References
~~~~~~~~~~

.. [ltc_datasheet] LTC6804 Datasheet http://cds.linear.com/docs/en/datasheet/680412fb.pdf
