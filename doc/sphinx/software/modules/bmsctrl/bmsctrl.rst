.. include:: ../../../macros.rst

.. _BMSCTRL:


===========
BMS Control
===========

.. highlight:: C

BMScontrol (|mod_bmsctrl|) handles requests of superior control units (e.g., via CAN messages) and checks for limits of voltage, current and temperature and monitors correct CAN timing.


Structure
~~~~~~~~~

:numref:`Fig. %s <bmsctrl_figure1>` shows the organization of the BMScontrol statemachine.

.. _bmsctrl_figure1:
.. figure:: BMSCTRL.png
   :width: 100 %

   BMScontrol state machine

Module Files
~~~~~~~~~~~~


Driver:
 - ``src\\engine\\bmsctrl\\bmsctrl.c``
 - ``src\\engine\\bmsctrl\\bmsctrl.h``
 
Driver Configuration:
 - ``src\\engine\\config\\bmsctrl_cfg.c``
 - ``src\\engine\\config\\bmsctrl_cfg.h``

.. _BMSCTRL_CONFIG:
 
BMSCTRL Configuration
~~~~~~~~~~~~~~~~~~~~~
 
|foxygen| configurable variables that define safe operating area:

==================  =========   ====   =========   ====   =========================================  ===============
NAME                LEVEL       TYPE   VALIDATOR   UNIT   DESCRIPTION                                standard value
==================  =========   ====   =========   ====   =========================================  ===============
BMSCTRL_TEMPMAX         devel   int    -40<x<80    °C     maximum temperature safe operating limit   55
BMSCTRL_TEMPMIN         devel   int     -40<x<80   °C     minimum temperature safe operating limit   -10
BMSCTRL_VOLTMAX         devel   int     0<x<5000   mV     maximum cell voltage safe operating limit  3600
BMSCTRL_VOLTMIN         devel   int     0<x<5000   mV     minimum cell voltage safe operating limit  2100
BMSCTRL_CURRENTMAX      devel   int          0<x   mA     maximum current safe operating limit       15000
BMSCTRL_CURRENTMIN      devel   int          x<0   mA     minimum current safe operating limit       -15000
==================  =========   ====   =========   ====   =========================================  ===============

These configuration values are building the main safety feature together with the derating (:ref:`SOX_CONFIG`) and are therefore
considered highly safety-relevant.

Example of safe operating area for lithium-ion LFP/Graphite chemistry:

This configuration is very conservative and limits are defensive. It is the default standard configuration for rechargeable lithium-ion batteries and has to be set up correctly to ensure safety. These values must be adapted carefully and specificly to the used battery cells.

================== ==== ===============  =========================================  
NAME               UNIT VALUE            DESCRIPTION                                
================== ==== ===============  =========================================  
BMSCTRL_TEMPMAX    °C   55               maximum temperature safe operating limit   
BMSCTRL_TEMPMIN    °C   -10              minimum temperature safe operating limit   
BMSCTRL_VOLTMAX    mV   3600             maximum cell voltage safe operating limit  
BMSCTRL_VOLTMIN    mV   2100             minimum cell voltage safe operating limit  
BMSCTRL_CURRENTMAX mA   15000            maximum current safe operating limit       
BMSCTRL_CURRENTMIN mA   -15000           minimum current safe operating limit       
================== ==== ===============  =========================================  

Example of safe operating area for lithium-ion NMC/LTO chemistry:

================== ==== ===============  =========================================  
NAME               UNIT VALUE            DESCRIPTION                                
================== ==== ===============  =========================================  
BMSCTRL_TEMPMAX    °C   55               maximum temperature safe operating limit   
BMSCTRL_TEMPMIN    °C   -30              minimum temperature safe operating limit   
BMSCTRL_VOLTMAX    mV   2600             maximum cell voltage safe operating limit  
BMSCTRL_VOLTMIN    mV   1700             minimum cell voltage safe operating limit  
BMSCTRL_CURRENTMAX mA   160000           maximum current safe operating limit       
BMSCTRL_CURRENTMIN mA   -160000          minimum current safe operating limit       
================== ==== ===============  =========================================  

Example of safe operating area for lithium-ion NCA/Graphite and NMC/Graphite chemistries:

================== ==== ===============  =========================================  
NAME               UNIT VALUE            DESCRIPTION                                
================== ==== ===============  =========================================  
BMSCTRL_TEMPMAX    °C   55               maximum temperature safe operating limit   
BMSCTRL_TEMPMIN    °C   -10              minimum temperature safe operating limit   
BMSCTRL_VOLTMAX    mV   4100             maximum cell voltage safe operating limit  
BMSCTRL_VOLTMIN    mV   2700             minimum cell voltage safe operating limit  
BMSCTRL_CURRENTMAX mA   210000           maximum current safe operating limit       
BMSCTRL_CURRENTMIN mA   -80000           minimum current safe operating limit       
================== ==== ===============  =========================================  

.. regex for extraction from header file
.. \s\*\s(.*)\r\n(\s\*\s(.*)(\r\n)*)\s\*\s@var\s(.*)\r\n\s\*\s@level\s(.*)\r\n\s\*\s@type\s(.*)\r\n\s\*\s@validator\s(.*)\r\n\s\*\s@unit\s(.*)\r\n\s\*\s@group\sBMSCTRL\r\n

|foxygen| configurable variables that switch off or on specific checks:

============================  =========   =========================================  ===============
NAME                          LEVEL       DESCRIPTION                                default value
============================  =========   =========================================  ===============
BMSCTRL_CAN_TIMING_TEST       user        CAN timing test enable                     TRUE
BMSCTRL_TEST_CELL_LIMITS      devel       Cell limits test enable                    TRUE
BMSCTRL_TEST_CELL_SOF_LIMITS  user        SOF limits test enable                     FALSE
============================  =========   =========================================  ===============

|foxygen| configurable IDs of requests receivable via CAN signal:

============================  =========   =========================================  ===============
NAME                          LEVEL       DESCRIPTION                                default value
============================  =========   =========================================  ===============
BMSCTRL_REQ_ID_NORMAL         user        ID to request for NORMAL state             3
BMSCTRL_REQ_ID_STANDBY        user        ID to request for STANDBY state            8
============================  =========   =========================================  ===============

Dependencies
~~~~~~~~~~~~
**DATABASE**
 - ``src\\engine\\database\\database.h`` (included indirectly by ``general.h``)
 
``BMSCTRL`` accesses minimum and maximum measurement values, SOF calculation results, can request data and can timing data from ``DATABASE``

**SYSCTRL**
 - ``src\\engine\\sysctrl\\sysctrl.h``
 
``BMSCTRL`` requests states of ``SYSCTRL`` statemachine depending on the input from CAN signals from ``DATABASE``.

**SOX**
 - ``src\\engine\\sox\\sox.h``
 
``BMSCTRL`` triggers the SOC initialization (until now only dummy implementation).

**MCU**
 - ``src\\module\\mcu\\mcu.h``

``BMSCTRL`` enables and disables interrupt functionality and gets timestamps from MCU module.

**CONT**
 - ``src\\module\\contactor\\contactor.h``

``BMSCTRL`` gets the feedback of the contactors.

Inputs and Outputs to BMSCTRL
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Called by 1ms Task:

Input:
 - current measurements
 - voltage measurement minimum and maximum
 - temperature measurement minimum and maximum
 - requests received from CAN bus
 - current derating values (i.e., SOF calculation results)
 
Output:
 - trigger to SOC initialization
 - requests to ``SYSCTRL`` statemachine
 
 