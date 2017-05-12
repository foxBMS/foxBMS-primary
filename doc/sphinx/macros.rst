:orphan:

.. -----------------------------------------------
.. General Documentation Macros
.. -----------------------------------------------
.. |version| replace:: ``0.4.4``
.. |foxbms| replace:: foxBMS
.. |foxconda| replace:: foxConda
.. |frontdesk| replace:: Front Desk
.. |foxygen| replace:: Foxygen
.. |doxygen| replace:: Doxygen

.. -----------------------------------------------
.. Haardware Documentation Macros
.. -----------------------------------------------
.. |LTC| replace:: LTC6804-1/LTC6811-1
.. |master| replace:: foxBMS Master Unit
.. |masters| replace:: foxBMS Master Units
.. |slave| replace:: foxBMS Slave Unit
.. |slaves| replace:: foxBMS Slave Units
.. |BMS-Master| replace:: BMS-Master board
.. |BMS-Extension| replace:: BMS-Extension board
.. |BMS-Interface| replace:: BMS-Interface board
.. |BMS-Slave| replace:: BMS-Slave board
.. |BMS-Slaves| replace:: BMS-Slave boards
.. |primary| replace:: primary
.. |secondary| replace:: secondary
.. |MCU0| replace:: MCU0
.. |MCU1| replace:: MCU1
.. |CAN0| replace:: CAN_0
.. |CAN1| replace:: CAN_1

.. -----------------------------------------------
.. Software Module Name Macros
.. -----------------------------------------------
.. |mod_bmsctrl| replace:: ``bmsctrl`` module
.. |mod_sysctrl| replace:: ``syscontrol`` module
.. |mod_io| replace:: ``io`` module
.. |mod_contactor| replace:: ``contactor`` module
.. |mod_can| replace:: ``can`` module
.. |mod_cansignal| replace:: ``cansignal`` module
.. |mod_ltc| replace:: ``ltc`` module
.. |mod_com| replace:: ``com`` module
.. |mod_uart| replace:: ``uart`` module
.. |mod_chksum| replace:: ``chksum`` module
.. |mod_sox| replace:: ``sox`` module
.. |mod_isoguard| replace:: ``isoguard`` module
.. |mod_diag| replace:: ``diag`` module
.. |primary archive| replace:: ``foxbms_primary-0-4-4.tar.gz``
.. |secondary archive| replace:: ``foxbms_secondary-0-4-4.tar.gz``
.. |installer archive| replace:: ``foxcondainstaller-0-4-3.exe``

.. -----------------------------------------------
.. Hardware-Software Quickcheck Macros
.. -----------------------------------------------
.. |eo| replace:: ``emergency off``
.. |il| replace:: ``interlock``
.. |pmc| replace:: ``plus main contactor``
.. |ppc| replace:: ``plus precharge contactor``
.. |mmc| replace:: ``minus main contactor``
.. |hear| replace:: *(H)*
.. |build| replace:: ``python Tools\waf-1.8.12 configure --check-c-compiler=gcc build``
.. |drive| replace:: Drive
.. |stdby| replace:: Standby

.. -----------------------------------------------
.. Variable Name Macros
.. -----------------------------------------------
.. |var_il| replace:: variable: ``cont_interlock_state``
.. |var_co| replace:: variable: ``cont_contactor_states``
.. |var_bkpsram_co| replace:: variable: struct ``bkpsram_ch_1`` member: ``contactors_count``
.. |var_sysctrl_st| replace:: variable ``sysctrl`` member: ``SYSCTRL_STATEMACH_e``

.. -----------------------------------------------
.. State Machine Macros
.. -----------------------------------------------
.. |req_drive| replace:: Request "Drive"-state via CAN
.. |req_stdby| replace:: Request "Standby"-state via CAN
.. -----------------------------------------------
