===============================
Recommended Additional Hardware
===============================

For a safe battery system, the following parts and components are recommended in addition to the foxBMS master.

-----------------
Isolation Monitor
-----------------
foxBMS supports an isolation monitor to detect faulty isolation of the HV battery against chassis or battery pack enclosure. Bender ISOMETER IR155-3203 or IR155-3204/-3210 are supported and tested, while IR155-3204 is recommended.

----------
Contactors
----------

For switching battery HV, contactors (power relays) should be used. Contactors with auxiliary contacts for state feedback are recommended. foxBMS was intensively tested with contactors type Gigavac GX14 (GX14BAB: 12V coil voltage and normally open auxiliary contacts), but designed to work many other contactors. In any case, please check the pick-up and continuous current of the contactor coil to be within the foxBMS electrical ratings. Nevertheless, contactors matching the used battery pack specification (e.g., continuous current, maximum voltage, short circuit current) should be used.

--------------
Current Sensor
--------------

For measuring the battery pack current, CAN based current sensors are supported. An Isabellenhuette IVT-Mod 300 shunt based current sensor is supplied with foxBMS. This automotive current sensor provides an current measurement range of +/-300A and in addition three voltage measurement inputs with an voltage measurement range of +/-600V. Current sensors with higher current measurement range are available, please check Isabellenhütte homepage or contact a sales person. Any other CAN based current sensor can be used, but foxBMS software might have to be adapted.

--------------------
DC Battery Pack Fuse
--------------------

When using foxBMS with batteries, a DC fuse in the HV path is mandatory. In case of external short circuit of the battery pack, this fuse opens the HV path. This DC fuse is a critical safety component and needs to be selected carefully. The different parameters of the specific battery pack used must be taken into account (e.g., maximum voltage, expected short circuit current, continuous current).

:numref:`Fig. %s <qsblockdiagram>` shows how to set up a basic battery system using these components. In the section :ref:`bjb_description`, details can be found on how to use these and other components depending on the used battery system.

.. _qsblockdiagram:
.. figure:: 2016-04-12_system.png
   :width: 100 %
   
   Battery system topology block diagram
   
 