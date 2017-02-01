===========================
foxBMS Hardware Description
===========================

This part of the documentation describes the hardware modules used in foxBMS.
:numref:`Fig. %s <hwblockdiagram>` shows a block diagram of the foxBMS master with all its components. 

.. _hwblockdiagram:
.. figure:: ./_figures/2016-04-06_foxBMS_Block_Diagram.png
   
   
   foxBMS master hardware block diagram

Supply 0 (PRIMARY)
------------------

.. _supplyzero:
.. figure:: ./_figures/2016-04-14_SUPPLY_0.png
   :width: 4356 px
   :scale: 20 %   
   
   Supply for all circuit parts related to the "primary" part of foxBMS

The external supply for the foxBMS master is divided in a part supplying the interlock circuit and the contactors (SUPPLY_EXT_2) and the rest of the circuit (SUPPLY_EXT_0) as shown in :numref:`Fig. %s <supplyzero>` . foxBMS may only be supplied with 12 to 24 V DC. The "primary" part of the foxBMS is isolated from SUPPLY_EXT_0 by an isolating DC/DC converter (IC202). The 5 V output of the DC/DC converter is stepped down to 3.3 V by an LM1085 LDO (IC201). The output of IC201 (+3.3V_0) supplies the primary microcontroller (MCU_0) and the related circuits. 

Supply 1 (SECONDARY)
--------------------

.. _supplyone:
.. figure:: ./_figures/2016-04-14_SUPPLY_1.png
   :width: 3619 px
   :scale: 20 %   
   
   Supply for all circuit parts related to the "secondary" part of foxBMS

The "secondary" part of foxBMS is also supplied by SUPPLY_EXT_0 as shown in :numref:`Fig. %s <supplyone>`. Also the "secondary" part of the foxBMS is isolated from SUPPLY_EXT_0 by an isolating DC/DC converter (IC302). The 5 V output of the DC/DC converter is stepped down to 3.3 V by an LM1085 LDO (IC301). The output of IC301 (+3.3V_1) supplies the secondary microcontroller (MCU_1) and the related circuits.

MCU 0 (PRIMARY)
---------------

.. _mcuzerobootrst:
.. figure:: ./_figures/2016-04-14_MCU_0_BOOT_RST.png
   :width: 2175 px
   :scale: 20 %   
   
   Primary MCU boot and reset circuit

:numref:`Fig. %s <mcuzerobootrst>` shows the boot and reset related circuits of the primary microcontroller MCU_0. MCU_0 can be manually reset by push button S401. Please note that the housing has to be opened to reach S401, therefore resetting MCU_0 by means of S401 is intended for use in a laboratory setting / debugging situation. 

.. _mcuzeroadcref:
.. figure:: ./_figures/2016-04-14_MCU_0_ADC_REF.png
   :width: 1481 px
   :scale: 20 %   
   
   Primary MCU ADC reference voltage 

The ADCs of the primary microcontroller MCU_0 are supplied with a 2.5 V reference voltage provided by an ADR3425 (IC401) as shown in  :numref:`Fig. %s <mcuzeroadcref>` .
   
.. _mcuzeroosc:
.. figure:: ./_figures/2016-04-14_MCU_0_OSC.png
   :width: 1925 px
   :scale: 20 %   
   
   Primary MCU clock circuits
   
The primary microcontroller MCU_0 is clocked by an 8 Mhz oscillator as shown in :numref:`Fig. %s <mcuzeroosc>` . An separate oscillator (Q402) is used to clock the RTC (real time clock) integrated in MCU_0.

..
  AUSKOMMENTIERT:
  MCU 1 (SECONDARY)
  -----------------
  
  Some hardware module description


Interface between MCU 0 and MCU 1
---------------------------------

.. _mcumcuif:
.. figure:: ./_figures/2016-04-14_MCU_TO_MCU_IF.png
   :width: 1962 px
   :scale: 20 %   
   
   Interface between primary and secondary MCU

Besides being linked over the common interlock line, the primary and secondary microcontroller also hare a common SPI data interface. The secondary microcontroller MCU_1 acts as the master in the SPI communication. The interface is isolated using an ADUM1401 as shown in :numref:`Fig. %s <mcumcuif>`. 
 
Interface to Bender ISOMETER
----------------------------
.. _bender:
.. figure:: ./_figures/2016-04-14_BENDER.png
   :width: 3663 px
   :scale: 20 %   
   
   Interface to the Bender ISOMETER

The foxBMS Master supports Bender ISOMETER IR155-3203 and IR155-3204/-3210. Two inputs are provided, one for a PWM coded diagnostic signal and one for a simple status signal (ON or NOK) as shown in :numref:`Fig. %s <bender>`. The Bender ISOMETER is supplied SUPPLY_EXT_0 and may be switched on or off (lowside) by the foxBMS Master. The input signals are limited to  level of 5 V with Zener diodes D701 and D702. In order adapt the interface for use with a IR155-3204/-3210 device, the solder jumper R705 has to be removed. The input signals are isolated from the microcontoller by an ADUM1301 (IC702). 

CAN 0
-----

.. _can:
.. figure:: ./_figures/2016-04-14_CAN_0.png
   :width: 2625 px
   :scale: 20 %   
   
   Circuit of the CAN interface (CAN 0) 

The CAN 0 interface is intended to connect additional sensors, such as the Isabellenhuette IVT-MOD-300 current sensor to the foxBMS Master and the foxBMS Master to other devices such as a test bench control unit or an HMI unit. The circuit in :numref:`Fig. %s <can>` shown the input circuit consisting of protection diode D801, common mode choke L801, C804, and termination resistors R801 and R802. The CAN transceiver TJA1052 provides isolation and can be put to sleep by the primary microcontroller MCU_0 via an  CPC1008N optocoupler (IC803). The external part of the CAN_0 interface is supplied by SUPPLY_EXT_0. 

Interlock
---------

.. _interlock:
.. figure:: ./_figures/2016-04-14_INTERLOCK.png
   :width: 4863 px
   :scale: 20 %   
   
   Interlock circuit 

The primary and secondary microcontroller share a common interlock line as shown in :numref:`Fig. %s <interlock>`. The interlock line is isolated from both microcontrollers by optocouplers. The interlock line is supplied with 10 mA by a current source (LM317 - IC901). It can be interrupted by the primary microcontroller MCU_0 via optocoupler IC902 and can be read back by MCU_0 via optocoupler IC903. The secondary microcontroller MCU_1 can interrupt the interlock line via IC904 and read the interlock status via IC905. The 10 mA cause a voltage drop on R906, which turns on MOSFET T901. T901 switches the common ground of all contactors (connected to the foxBMS Master and Extension board). Therefore, when the interlock line is interrupted, the contactors are no longer supplied and open. 

Contactors
----------

.. _contactors:
.. figure:: ./_figures/2016-04-14_CONTACTORS.png
   :width: 4683 px
   :scale: 20 %   
   
   Contactor circuit, exemplariliy shown for contactor 0 

The foxBMS Master can control up to 9 contactors. The according control and feedback circuit is exemplarily shown  for contactor 0 in :numref:`Fig. %s <contactors>`. The contactor is switched on and off by an AQV25G2S OPTOMOS (IC1001) by the primary microcontroller MCU_0. Every contactor channel is protected with slow blowing fuse (F1001) type Schurter UMT-250 630mA (3403.0164.xx). The free wheeling diode D1001 is not populated. It has to be inserted when contactors are used, that do not provide an internal free wheeling diode. The contactor interface also supports a feedback functionality for contactors with auxiliary contacts. The contactor status can be read back by MCU_0 via an ADUM1300 (IC1103). 

Isolated USB interface (PRIMARY and SECONDARY)
----------------------------------------------

.. _usb:
.. figure:: ./_figures/2016-04-14_USB.png
   :width: 2750 px
   :scale: 20 %   
   
   USB interface circuit 

Both mircocontroller provide an isolated USB interface, as exemplarily shown for MCU_0 in :numref:`Fig. %s <usb>`. A FT231XS-R interface IC (IC1402) converts the USB signal to UART, which can easily be interfaced by the microcontroller. The UART signals are isolated by an ADUM1401 isolation IC (IC1403). The USB interface can be used to flash the microcontroller and for communication. 

EEPROM
------

.. _eeprom:
.. figure:: ./_figures/2016-04-14_EEPROM.png
   :width: 2375 px
   :scale: 20 %   
   
   EEPROM, exemplarily shown for the MCU_0 

The foxBMS master provides an 128kB EEPROM for data storage for the primary and secondary MCU (see :numref:`Fig. %s <eeprom>`). It uses an SPI interface, which is shared with the SD-Card, which is also connected to MCU_0. 


Isolated RS485 Interface
------------------------

.. _rs485:
.. figure:: ./_figures/2016-04-07_RS485.png
   :width: 3469 px
   :scale: 20 %
   
   Isolated RS485 interface circuit

On the master extension board an isolated RS458 interface can be found. It can be used to communicate with the BMS, as an alternative to the CAN interface or the UART over USB interface. Moreover, via this interface monitoring circuits (slaves) using RS485 instead of CAN or a proprietary communication protocol can be connected to the master. :numref:`Fig. %s <rs485>` shows the RS485 interface schematic. The external part of the circuit is supplied via a voltage applied to pins 5 (7 - 20V) and 6 (GND) of connector X1301 . The external supply voltage is also available on pin 1 (GND) and pin 4 . IC1301 provides 5V supply voltage for the transceiver (IC1302) and the external side of the isolator (IC1303). The transceiver (LT1785) features a receiver enable (!RE) and a driver enable (DE) functionality, which can be controlled by the primary microcontroller via the signals RS485_MCU_0_NRE and RS485_MCU_0_DE respectively. For data transmission the signals RS485_MCU_0_TX and RS485_MCU_0_RX are used. The data signals are available on connector X1301 pins 1 and 2. The data signals and the enable signals are galvanically isolated from the BMS master circuit by an ADUM1401 isolator IC.  


Isolated Normally Open Contact (isoNOC) Interface
---------------------------------------------------

.. _isonoc:
.. figure:: ./_figures/2016-04-07_iso_NOC.png
   :width: 1950 px
   :scale: 20 %
   
   Isolated normally open contacts interface (ISONOC_0 examplarily)

The BMS master extension board provides 6 normally open contacts (ISONOC_0 to ISONOC_5) for multi purpose use. Their function is exemplarily described for ISONOC channel 0 ( shown in :numref:`Fig. %s <isonoc>` ). Isolation and switching functionality are realized by AQV25G2S optoMOS (IC2001) . The optoMOS are controlled by a MOSFET (T2001), which again is switched by the primary microcontroller (ISONOC_0_CONTROL). The optoMOS is configured for a maximum load current of 6 A at 50V. Diode D1002 is optionally and not populated by default. Both power terminals of the optoMOS are available on connector X2001 as ISONOC_0_POSITIVE and ISONOC_0_POSITIVE on consecutive pins 1 and 2. 
   
Analog Input
------------

.. _analoginput:
.. figure:: ./_figures/2016-04-07_analog_input.png
   :width: 3238 px
   :scale: 20 %
   
   Analog input (not isolated) (analog channel 0 exemplarily)

For the acquisition of analog data, there are 5 ADC channels (ANALOG_IN_CH_0 - ANALOG_IN_CH_4) available on the master extension board. :numref:`Fig. %s <analoginput>` shows the input circuit for channel 0. The analog input of the microcontroller (ADC_MCU_0_CH_0) is protected by diode D1701, which clamps the input voltage to 3,3V. By default R1701 is populated with a 0 Ohm jumper, while R1702 is 7,75 kOhm and C1701 is 100 nF. The analog input channels are available on connector X1701. A reference voltage of 2,5 V is provided by IC1701 (ADR3425), which can supply a total load current up to +10 mA and sink up −3 mA. The reference voltage is available in X1701 next to every analog input pin. Pin 11 and 12 are connected to GND. Pleas note that the analog input are not isolated. They are referenced to the same potential as the primary microcontroller. 

   
Isolated GPIO
-------------

.. _isogpio:
.. figure:: ./_figures/2016-04-07_isolated_GPIO.png
   :width: 3425 px
   :scale: 20 %
   
   Isolated GPIOs (Input 0 and 1; Output 0 and 1 shown exemplarily)

The BMS master extension board provides 4 isolated inputs and 4 isolated outputs for general purpose (shown in :numref:`Fig. %s <isogpio>` ). Two ADUM1402 (IC1902 and IC1903) are used for isolation. Their external side of is supplied by SUPPLX_EXT_0 via a 78L05F voltage regulator (IC1901). The inputs are equipped with a 10k pull down resistor. All isolated GPIOs are available on the connector X1901 pins 1 to 8. Pins 9 and 10 of X1901 are connectrd to GND_EXT_0. 
   
SD Card
-------

.. _sdcard:
.. figure:: ./_figures/2016-04-07_SD_Card.png
   :width: 2056 px
   :scale: 20 %
   
   SD Card

On the foxBMS master extension board also a SD card slot can be found. It is directly connected to the Data Storage SPI of the promary microcontroller. :numref:`Fig. %s <sdcard>`  shows the schematic. Via the signal SDCARD_SUPPLY_CONTROL (primary microcontroller) the supply voltage of the SD card can be switched on and off. 
