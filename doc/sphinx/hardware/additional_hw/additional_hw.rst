.. include:: ../../macros.rst

===============================
Recommended Additional Hardware
===============================

Before using |master| and the |slaves| with lithium-ion batteries, safety must be considered carefully. The following parts and components are recommended to be used in addition to the |master| and the |slaves|.

-----------------
Isolation Monitor
-----------------
|foxBMS| supports an isolation monitor to detect faulty isolation of the battery pack against chassis or battery pack enclosure. Bender ISOMETER IR155-3203/-3204/-3210 are supported and tested, while IR155-3204 is recommended.

----------------
Power Contactors
----------------

To switching high-current capable batteries, mechanical power contactors (power relays) shall be used. For safety reasons, contactors with auxiliary contacts providing true state feedback are strongly recommended. |foxBMS| was developed and tested with the contactors of type Gigavac GX14 (GX14BAB: 12V coil voltage and normally open auxiliary contacts), but will probably work with many other contactors. The most important safety parameters are the maximum break current and the maximum allowed voltage to be broken. This must carefully be checked by the user of the |foxBMS| research and development platform. Further, the pick-up and continuous current of the contactor coil must be checked to be within the |foxBMS| electrical ratings. Nevertheless, contactors matching the used battery pack specification (e.g., continuous current, maximum voltage, short circuit current) are mandatory.

--------------
Current Sensor
--------------

To measure the battery pack current, CAN based current sensors can be used. The Isabellenhütte IVT-MOD and IVT-S shunt based current sensors were tested with |foxBMS|. The automotive current sensor IVT-MOD 300 provides a current measurement range of +/-300A and in addition three voltage measurement inputs with a voltage measurement range of +/-600V. Current sensors with higher current measurement ranges are available. Please check the `Isabellenhütte homepage <https://www.isabellenhuette.de/en/home>`_ and contact a `salesperson <https://www.isabellenhuette.de/en/contact>`_ to get more information. If another CAN based current sensor is used, the embedded software on the |BMS-Master| must be adapted.

--------------------
DC Battery Pack Fuse
--------------------

When using |foxBMS| with batteries, a DC fuse in the current path is mandatory. In case of external short circuit of the battery pack, this fuse must be able to break the current at the maximum high-voltage of the complete battery. This DC fuse is a critical safety component and needs to be selected very carefully. The different parameters of the specific battery pack used must be taken into account (e.g., maximum voltage, expected short circuit current, continuous current).

:numref:`Fig. %s <qsblockdiagram>` shows how to set up a basic battery system using these components. In the section :ref:`bjb_description`, details can be found on how to use these and other components depending on the used battery system.

.. _qsblockdiagram:
.. figure:: 2016-04-12_system.png
   :width: 100 %
   
   Block diagram showing the typical topology of a battery system
   
 