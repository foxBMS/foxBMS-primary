.. _hw_short_spec:

============================
Hardware Short Specification
============================

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

These specifications are needed to work further with the |foxBMS| hardware.

------------------
Electrical Ratings
------------------

==================================================  =======     =======     =======     ====
Description                                         Minimum     Typical     Maximum     Unit
==================================================  =======     =======     =======     ====
Supply Voltage DC                                   10          --          26          V
Contactor Continuous Current                         --          --          4           A
Contactor Feedback Supply Voltage                   --          5.0         --          V
Analog Input                                        --          --          3.3         V
Isolated Contacts Continuous Current                --          --          4           A
Interlock Circuit Sink Current                      --          10          --          mA
foxBMS Idle Supply Current at 12V supply            --          150         --          mA
foxBMS Idle Supply Current at 24V supply            --          110         --          mA
==================================================  =======     =======     =======     ====

----------
Connectors
----------

This section describes the pin out of all connectors on the |foxBMS Master|. 

.. _foxbmsfront:
.. figure:: ./_figures/2016-04-13_foxBMS_Front.png
   :width: 100 %
   
   Front view of the foxBMS Master, indicating the location of each header

|foxBMS| uses only Molex Micro-Fit 3.0 type connectors, except for USB. A comprehensive set of connectors and crimps is supplied with |foxBMS| to start connecting immediately. In case crimps or housings are missing, they are commonly available at major distributors. 

Molex Micro-Fit 3.0 Crimps Part Number: 46235-0001 (Farnell 2284551)

Molex Micro-Fit 3.0 Connectors:

==========   =========================   =====================
Pin Count    Molex Housing Part Number    Farnell Order Number
==========   =========================   =====================
2            43025-0200                  672889
4            43025-0400                  672890
6            43025-0600                  672907
10           43025-1000                  672920
12           43025-1200                  629285
16           43025-1600                  9961321
==========   =========================   =====================

.. figure:: ./_figures/2016-04-12_sixcon_view.png
   :width: 100 %

   Defined viewing direction for the connector pin out; receptable - rear view; header - front view (image source: MOLEX)

--------------------------------------------
Supply (X201 on |foxBMS Master Basic board|)
--------------------------------------------

.. figure:: ./_figures/2016-04-12_sixcon.png
   :width: 10 %

====   =============    ============    ============   
Pin    Signal           Direction       Description
====   =============    ============    ============   
1      SUPPLY_EXT_2     Input           12 - 24V 
2      SUPPLY_EXT_2     Input           12 - 24V
3      GND_EXT_2        Input           GND
4      SUPPLY_EXT_0     Input           12 - 24V
5      GND_EXT_0        Input           GND
6      GND_EXT_2        Input           GND
====   =============    ============    ============

* SUPPLY_EXT_0 / GND_EXT_0: Microcontroller supply and isolation devices supply 
* SUPPLY_EXT_2 / GND_EXT_2: Contactor supply and interlock supply 

--------------------------------------------
CAN 0  (X801 on |foxBMS Master Basic board|)
--------------------------------------------

.. figure:: ./_figures/2016-04-12_fourcon.png
   :width: 250 px
   :scale: 20 %

====    =========    =============    ============
Pin     Signal       Direction        Description
====    =========    =============    ============
1       GND_EXT_0    Output
2       NC           --
3       CAN_0_L      Input/Output
4       CAN_0_H      Input/Output
====    =========    =============    ============

Ground of CAN 0 is shared with supply ground GND_EXT_0. CAN  bus 0 is isolated from the MCU via the isolated CAN transceiver TJA1052. The CAN transceiver may be put into standby mode by MCU_0. 

-----------------------------------------------
CAN 1 (X801 on |foxBMS Master Extension board|)
-----------------------------------------------

.. figure:: ./_figures/2016-04-12_fourcon.png
   :width: 250 px
   :scale: 20 %

====    ============    =============    ============
Pin     Signal          Direction        Description
====    ============    =============    ============
1       GND_EXT_1       Input
2       SUPPLY_EXT_1    Input
3       CAN_1_L         Input/Output
4       CAN_1_H         Input/Output
====    ============    =============    ============

CAN 1 has to be supplied externally (GND_EXT_1 / SUPPLY_EXT_1) with 12 - 24V. CAN bus 1 is isolated from the MCU via the isolated CAN transceiver TJA1052. The CAN transceiver may be put into standby mode by MCU_0. 

--------------------------------------------------------------------------
Isolation Monitor (Bender ISOMETER) (X701  on |foxBMS Master Basic board|)
--------------------------------------------------------------------------

.. figure:: ./_figures/2016-04-12_fourcon.png
   :width: 250 px
   :scale: 20 %

====    =======================    ==============    ===================================================
Pin     Signal                     Direction         Description
====    =======================    ==============    ===================================================
1       BENDER_NEGATIVE_SUPPLY     Output            Supply to isolation monitoring device
2       SUPPLY_EXT_0               Output            Supply to isolation monitoring device
3       BENDER_OK_EXT              Input             Status signal of isolation monitoring device
4       BENDER_PWM_EXT             Input             Isolation monitoring device diagnostic signal
====    =======================    ==============    ===================================================

This interface is intended to be used with a Bender isolation monitoring device . Bender ISOMETER IR155-3203 and IR155-3204/-3210 are supported. The Bender ISOMETER is supplied and may be switched on or off (lowside) by the foxBMS Master. By factory, foxBMS is configured to work with Bender ISOMETER IR155-3204/-3210. In order to work with Bender ISOMETER IR155-3203, Jumper R705 must be removed on foxBMS Basic Board. For details, check the schematic of foxBMS Master. 

--------------------------------------------------------------------------------------------------------------
Contactors (X1201 - X1206 on |foxBMS Master Basic board| and X1201 - X1203 on |foxBMS Master Extension board|)
--------------------------------------------------------------------------------------------------------------

.. figure:: ./_figures/2016-04-12_fourcon.png
   :width: 250 px
   :scale: 20 %

====    ========================    ==============    ============================================
Pin     Signal                      Direction         Description
====    ========================    ==============    ============================================
1       GND_EXT_0                   --                Contactor auxiliary contact
2       CONTACTOR_X_FEEDBACK_EXT    --                Contactor auxiliary contact
3       CONTACTOR_X_COIL_POS        Output            Positive contactor coil supply
4       CONTACTORS_COMMON_NEG       Output            Negative contactor coil supply
====    ========================    ==============    ============================================

All contactor connectors share one common ground. This common ground is switched lowside by the interlock circuit. Opening the interlock loop deactivates the contactor supply and opens all contactors immediately. A contactor auxiliary contact may be read by connecting the auxiliary contact to the corresponding pins of the contactor connector.

Optional freewheeling diodes are not populated on the PCB. If the used contactor does not have built-in freewheeling diodes, freewheeling diodes must be added to protect the contactor control circuit. The load current is limited by the optically isolated power switch AQV25G2S (6A continuous load). Every contactor connector is fused with an onboard slow blow fuse type Schurter UMT-250 630mA (3403.0164.xx). 

-----------------------------------------------
Interlock (X901 on |foxBMS Master Basic board|)
-----------------------------------------------

.. figure:: ./_figures/2016-04-12_twocon.png
   :width: 131 px
   :scale: 20 %

====    ========================    ==============        ============================================
Pin     Signal                      Direction             Description
====    ========================    ==============        ============================================
1       INTERLOCK_IN                Input                 --
2       INTERLOCK_OUT               Output                --
====    ========================    ==============        ============================================

The interlock circuit has a built-in current source, adjusted to 10mA constant current. In fault conditions, all contactors are opened immediately by opening the interlock circuit. If the interlock circuit is externally opened, the contactor supply is deactivated immediately. This circuit has no effect on the foxBMS supply or communication interfaces. 

--------------------------------------------------------------------------
Daisy Chain - Primary and Secondary (X1601 on |foxBMS Master Basic board|)
--------------------------------------------------------------------------

.. figure:: ./_figures/2016-04-12_sixteencon.png
   :width: 981 px
   :scale: 20 %

====   ====================== 
Pin    Signal                    
====   ======================
1      NC       
2      OUT+ (Secondary |LTC|)  
3      OUT- (Secondary |LTC|)  
4      NC       
5      NC       
6      OUT+ (Primary |LTC|)    
7      OUT- (Primary |LTC|)    
8      NC                     
9      NC                    
10     NC                      
11     NC                      
12     NC                     
13     NC                     
14     NC                           
15     NC                                    
16     NC                               
====   ======================

Please note: This connector pin out is only valid for use of a foxBMS Master Interface board for the LTC6802 monitoring IC. 

------------------------------------------------
RS485 (X1301 on |foxBMS Master Extension board|)
------------------------------------------------

.. figure:: ./_figures/2016-04-12_sixcon.png
   :width: 369 px
   :scale: 20 %
   
====   =============    ============    ============   
Pin    Signal           Direction       Description
====   =============    ============    ============   
1      GND_EXT_2        Output
2      RS485_A          Input/Output
3      RS485_B          Input/Output
4      SUPPLY_EXT_2     Output
5      SUPPLY_EXT_2     Input
6      GND_EXT_2        Input
====   =============    ============    ============

The RS485 interface uses the ESD rugged transceiver LT1785. Moreover, the interface is galvanically isolated. An external supply has to be provided (12 - 24V). 

--------------------------------------------------------
Isolated GPIO (X1901 on |foxBMS Master Extension board|)
--------------------------------------------------------

.. figure:: ./_figures/2016-04-12_tencon.png
   :width: 625 px
   :scale: 20 %
   
====   =============    ============    ============   
Pin    Signal           Direction       Description
====   =============    ============    ============   
1      ISOGPIO_IN_0     Input
2      ISOGPIO_IN_1     Input
3      ISOGPIO_IN_2     Input
4      ISOGPIO_IN_3     Input
5      ISOGPIO_OUT_0    Output
6      ISOGPIO_OUT_1    Output
7      ISOGPIO_OUT_2    Output
8      ISOGPIO_OUT_3    Output
9      GND_EXT_0        Output
10     GND_EXT_0        Output
====   =============    ============    ============

The foxBMS Master Extension board provides 4 isolated general purpose inputs and 4 isolated general purpose outputs. The GPIOs are isolated by an ADUM1402. The inputs are equipped with 10kOhm pull down resistors and are intended for a maximum input voltage of 5V. The output voltage is also 5V. An external supply is not needed.  

-----------------------------------------------------------------------------------
Isolated normally open contacts - isoNOC (X2001 on |foxBMS Master Extension board|)
-----------------------------------------------------------------------------------

.. figure:: ./_figures/2016-04-12_twelvecon.png
   :width: 744 px
   :scale: 20 %
   
====   ====================    ============    ============   
Pin    Signal                  Direction       Description
====   ====================    ============    ============   
1      ISONOC_0_POSITIVE       
2      ISONOC_1_POSITIVE       
3      ISONOC_2_POSITIVE       
4      ISONOC_3_POSITIVE       
5      ISONOC_4_POSITIVE       
6      ISONOC_5_POSITIVE       
7      ISONOC_0_NEGATIVE       
8      ISONOC_1_NEGATIVE       
9      ISONOC_2_NEGATIVE       
10     ISONOC_3_NEGATIVE       
11     ISONOC_4_NEGATIVE       
12     ISONOC_5_NEGATIVE       
====   ====================    ============    ============

The foxBMS Master Extension board features 6 isolated normally open contacts. The load current of each channel is limited by the optically isolated power switch AQV25G2S. The channels are not fused, however freewheeling diodes type GF1B are installed on board. 

----------------------------------------------------
Analog Inputs (X1701 on |foxBMS Master Basic board|)
----------------------------------------------------

.. figure:: ./_figures/2016-04-12_twelvecon.png
   :width: 744 px
   :scale: 20 %
   
====   ====================    ============    ============   
Pin    Signal                  Direction       Description
====   ====================    ============    ============   
1      V_REF                   Output
2      ANALOG_IN_CH_0          Input
3      V_REF                   Output
4      ANALOG_IN_CH_1          Input
5      V_REF                   Output
6      ANALOG_IN_CH_2          Input
7      V_REF                   Output
8      ANALOG_IN_CH_3          Input
9      V_REF                   Output
10     ANALOG_IN_CH_4          Input
11     GND_0                   Output
12     GND_0                   Output
====   ====================    ============    ============

On the foxBMS BMS-Master Extension board 4 unisolated analog inputs to MCU_0 are available. For applications using NTCs, also a reference voltage of 2.5V is provided. The maximum input voltage is limited to 3.3V (Zener protected). For further information on the input circuit, please refer to the circuit description in the hardware user manual or the foxBMS Master schematic.   
