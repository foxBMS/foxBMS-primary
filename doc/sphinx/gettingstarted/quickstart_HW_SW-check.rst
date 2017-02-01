==================================
Hardware-Software Quickcheck Guide
==================================

.. -----------------------------------------------
.. General Documentation Macros
.. -----------------------------------------------
.. |foxBMS| replace:: foxBMS
.. |LTC| replace:: LTC6804-1
.. |foxBMS Master| replace:: foxBMS Master
.. |foxBMS Master Basic board| replace:: foxBMS Master Basic board
.. |foxBMS Master Extension board| replace:: foxBMS Master Extension board
.. |foxBMS Interface board| replace:: foxBMS Interface board
.. |foxBMS Slave| replace:: foxBMS Slave
.. |foxBMS Slave board| replace:: foxBMS Slave board

.. -----------------------------------------------
.. Hardware-Software Quickcheck Macros
.. -----------------------------------------------
.. |eo| replace:: ``emergency off``
.. |il| replace:: ``interlock``
.. |pmc| replace:: ``plus main contactor``
.. |ppc| replace:: ``plus precharge contactor``
.. |mmc| replace:: ``minus main contactor``
.. |version| replace:: ``0.4``
.. |hear| replace:: *(H)*
.. |build| replace:: ``python Tools\waf-1.8.12 configure --check-c-compiler=gcc build``
.. |drive| replace:: Drive
.. |stdby| replace:: Standby
.. -----------------------------------------------
.. variable name macros:
.. -----------------------------------------------
.. |var_il| replace:: variable: ``cont_interlock_state``
.. |var_co| replace:: variable: ``cont_contactor_states``
.. |var_bkpsram_co| replace:: variable: struct ``bkpsram_ch_1`` member: ``contactors_count``
.. |var_sysctrl_st| replace:: variable ``sysctrl`` member: ``SYSCTRL_STATEMACH_e``
.. -----------------------------------------------
.. state machine macros
.. -----------------------------------------------
.. |req_drive| replace:: Request "Drive"-state via CAN
.. |req_stdby| replace:: Request "Standby"-state via CAN
.. -----------------------------------------------

This section describes how to test the |foxBMS Master Basic board| with one |foxBMS Slave board| connected. These
tests shows if the following hardware and software components are working correctly:

 - |foxBMS Slave|
 
   - Hardware
   - Software
   
     - LTC driver
     - SPI driver

 - |foxBMS Master Basic board|
   
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

 - |foxBMS Master Basic board|
 - Supply cable
 - one |foxBMS Slave board| (and additional voltage divider for voltage simulation)
 - CAN-Bus to PC (e.g., PCAN USB)
 - Normally closed switch for opening the |il| (hereafter referred to as |eo|) 
 - 3 contactors with normally open feedback (hereafter referred to as |pmc|, |ppc|, |mmc|)
 - Debugger


Testing Without Debugger
------------------------

It is also possible to get little information if the system is running correctly without variable checking on the debugger.
When requesting the later described states to the BMS, one can hear the contactors opening and closing. 

-----------------
Required Software
-----------------

- |foxBMS|-SW for Primary
- |foxBMS|-SW for Secondary
- Debugger
- CAN-Bus to PC
- CAN message for |drive| and |stdby|

--------------
Test Procedure
--------------

The test procedure consists of two steps:

 #. preparing the hardware
 #. building the software from source
 #. requesting bms-states to |foxBMS Master Basic board| and checking variable values on the debugger

If one has no debugger, one can only test partially if all parts of |foxBMS| are running correctly. As mentioned above one is limited to hearing if the 
contactors are opening and closing. This is marked in the `Test`_ with |hear| and a following explanation if needed.

Hardware Setup
--------------

#. Use a voltage divider for voltage simulation at the LTC and connect it to the foxBMS slave.
#. Connect the two daisy chain connectors from slave to master. A daisy chain of 2 is faked.
#. Connect the |eo| to the |il| of the |foxBMS Master|.
#. Connect the following contactors to the |foxBMS Master|:

   #. ``contactor 0`` the |pmc| in the positive current path
   #. ``contactor 1`` the |ppc| in the positive current path
   #. ``contactor 2`` the |mmc| in the negative current path

#. The debugger is connected from the PC to the JTAG-interface of the primary controller of |foxBMS Master Basic board|.
#. Connect the CAN-interface

Software Setup
--------------

#. Build |foxBMS| version |version| with |build|.

Test
----

#. Power |foxBMS|
#. Flash |foxBMS| Primary
#. Start PCAN-View
#. Start |foxBMS|
#. |req_stdby|
#. Check on the debugger if system timer is running; variable: ``os_timer``
#. Check if |foxBMS Slave board| reads voltages; variable: ``ltc_cellvoltage``
#. |req_stdby|

   #. Check on the debugger if |il| is closed; |var_il| (cont_interlock_state.set = 1)
   
#. |req_drive|

   #. Check on the debugger if contactors are closed in the correct order; |var_co|

      #. close |mmc|
      #. close |ppc|
      #. close |pmc|
      #. open |ppc|

   |hear|: If this is test is performed with no debugger, one hears the contactors four times clicking in sum.

   #. Check contactor counter; |var_bkpsram_co|

#. |req_stdby|

   #. Check on the debugger if |il| is closed; |var_il|
   #. Check on the debugger if contactors are opened in the correct order; |var_co|
   
      #. Open |pmc|
      #. Open |mmc|
      
   |hear|: If this is test is performed with no debugger, one hears the contactors clicking once (too fast to hear the two contactors clicking separately).
   
   #. Check contactor counter; |var_bkpsram_co|
   
#. |req_drive|

   #. Check on the debugger if contactors are closed in the correct order; |var_co|

      #. close |mmc|
      #. close |ppc|
      #. close |pmc|
      #. open |ppc|

   |hear|: If this is test is performed with no debugger, one hears the contactors four times clicking in sum.

   #. Check contactor counter; |var_bkpsram_co|
   
#. Open |il| by pressing |eo|

   #. Check on the debugger if |il| is opened; |var_il|
   #. Check on the debugger if contactors are opened in the correct order; |var_co|

      #. Open |pmc|
      #. Open |mmc|
   
   |hear|: If this is test is performed with no debugger, one hears the contactors clicking once (too fast to hear the two contactors clicking separately).
   
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

   |hear|: If this is test is performed with no debugger, one hears the contactors four times clicking in sum.

   #. Check contactor counter; |var_bkpsram_co|

#. |req_stdby|

   #. Check on the debugger if |il| is closed; |var_il|
   #. Check on the debugger if contactors are still open; |var_co|

#. Power |foxBMS| off
#. Power |foxBMS|
#. Break  on ``main()`` with debugger
#. Distort contactor counter in SRAM; |var_bkpsram_co|
#. Go on/release break point

   #. Check the debugger if contactor counter values get reloaded from EEPROM; |var_bkpsram_co|

#. done