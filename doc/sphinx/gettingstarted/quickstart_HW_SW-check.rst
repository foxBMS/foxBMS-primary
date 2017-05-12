.. include:: ../macros.rst

.. _quickchek:

==================================
Hardware-Software Quickcheck Guide
==================================

This section describes how to test the |BMS-Master| with one |BMS-Slave| connected. These tests shows if the hardware and software components are working correctly:

 - |BMS-Slave|
 
   - Hardware
   - Software
   
     - LTC driver
     - SPI driver

 - |BMS-Master|
   
   - Hardware
   - Software

     - operating system
     - ``syscontrol`` and ``bmscontrol``
     - |il| and contactors
     - SRAM and EEPROM
     - CAN, SPI
     - diagnosis

-----------------
Required Hardware
-----------------

 - |BMS-Master|
 - Supply cable
 - one |BMS-Slave| (and additional voltage divider for voltage simulation)
 - CAN-Bus to PC (e.g., PCAN USB)
 - Normally closed switch for opening the |il| (hereafter referred to as |eo|) 
 - 3 contactors with normally open feedback (hereafter referred to as |pmc|, |ppc|, |mmc|)
 - Debugger


Testing Without Debugger
------------------------

It is also possible to get little information if the system is running correctly without variable checking on the debugger. When requesting the later described states to the BMS, the contactors opening and closing can be hear.

-----------------
Required Software
-----------------

- |foxbms| firmware for |MCU0|
- |foxbms| firmware for |MCU1|
- Debugger
- CAN-Bus to PC
- CAN message for |drive| and |stdby|

--------------
Test Procedure
--------------

The test procedure consists of two steps:

 #. preparing the hardware
 #. building the software from source
 #. requesting bms-states to |BMS-Master| and checking variable values on the debugger

If no debugger is available, only a partially test if all parts of |foxbms| are running correctly can be performed. As mentioned above, it can be checked accoustically if the 
contactors are opening and closing. This is marked in the `Test`_ with |hear| and a following explanation if needed.

Hardware Setup
--------------

#. Use a voltage divider for voltage simulation at the |LTC| and connect it to the |BMS-Slave|.
#. Connect the two daisy chain connectors from the |BMS-Slave| to the |BMS-Master|. A daisy chain of 2 |slaves| is simulated.
#. Connect the |eo| to the |il| of the |BMS-Master|.
#. Connect the following contactors to the |BMS-Master|:

   #. ``contactor 0`` the |pmc| in the positive current path
   #. ``contactor 1`` the |ppc| in the positive current path
   #. ``contactor 2`` the |mmc| in the negative current path

#. The debugger is connected from the PC to the JTAG-interface of |MCU0| of |BMS-Master|.
#. Connect the CAN-interface

Software Setup
--------------

#. Build |foxbms| version |version| with |build|.

Test
----

#. Power |foxbms|
#. Flash |foxbms| |MCU0|
#. Start PCAN-View
#. Start |foxbms|
#. |req_stdby|
#. Check on the debugger if the system timer is running; variable: ``os_timer``
#. Check if |BMS-Slave| reads voltages; variable: ``ltc_cellvoltage``
#. |req_stdby|

   #. Check on the debugger if |il| is closed; |var_il| (cont_interlock_state.set = 1)
   
#. |req_drive|

   #. Check on the debugger if contactors are closed in the correct order; |var_co|

      #. close |mmc|
      #. close |ppc|
      #. close |pmc|
      #. open |ppc|

   |hear|: If this test is performed with no debugger, contactors should be heard clicking four times.

   #. Check contactor counter; |var_bkpsram_co|

#. |req_stdby|

   #. Check on the debugger if |il| is closed; |var_il|
   #. Check on the debugger if contactors are opened in the correct order; |var_co|
   
      #. Open |pmc|
      #. Open |mmc|
      
   |hear|: If this test is performed with no debugger, each contactor should be heard clicking one time (too fast to hear the two contactors clicking separately).
   
   #. Check contactor counter; |var_bkpsram_co|
   
#. |req_drive|

   #. Check on the debugger if contactors are closed in the correct order; |var_co|

      #. close |mmc|
      #. close |ppc|
      #. close |pmc|
      #. open |ppc|

   |hear|: If this test is performed with no debugger, contactors should be heard clicking four times.

   #. Check contactor counter; |var_bkpsram_co|
   
#. Open |il| by pressing |eo|

   #. Check on the debugger if |il| is opened; |var_il|
   #. Check on the debugger if contactors are opened in the correct order; |var_co|

      #. Open |pmc|
      #. Open |mmc|
   
   |hear|: If this test is performed with no debugger, each contactor should be heard clicking one time (too fast to hear the two contactors clicking separately).
   
#. |req_drive|

   #. BMS should switch to ``SYSCTRL_STATE_ERROR`` as the |il| is still open; |var_sysctrl_st|
   
#. Close |il| by releasing |eo|

   #. Check on the debugger if |il| is still open; |var_il|
   #. BMS has to stay in error state: |var_sysctrl_st| has to stay in ``SYSCTRL_STATE_ERROR``

#. |req_stdby|

   #. Check on the debugger if |il| is closed; |var_il|
   #. Check on the debugger if contactors are still open; |var_co|

#. |req_drive|

   #. Check on the debugger if contactors are closed in the correct order; |var_co|

      #. close |mmc|
      #. close |ppc|
      #. close |pmc|
      #. open |ppc|

   |hear|: If this test is performed with no debugger, contactors should be heard clicking four times.

   #. Check contactor counter; |var_bkpsram_co|

#. |req_stdby|

   #. Check on the debugger if |il| is closed; |var_il|
   #. Check on the debugger if contactors are still open; |var_co|

#. Power |foxbms| off
#. Power |foxbms|
#. Break  on ``main()`` with debugger
#. Distort contactor counter in SRAM; |var_bkpsram_co|
#. Go on/release break point

   #. Check the debugger if the contactor counter values get reloaded from EEPROM; |var_bkpsram_co|

#. done
