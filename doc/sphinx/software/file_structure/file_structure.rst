Software File Structure
=======================

.. include:: ../../macros.rst

.. highlight:: C

The file structure of |foxBMS| is presented in the following diagram. The ``config`` directories contains configuration files for the software modules.
For instance for ltc, the ``ltc`` directory contains the files ``ltc.c`` and ``ltc.h``. The corresponding configuration files in the ``config`` directory
are ``ltc_cfc.c`` and ``ltc_cfg.h``.

.. code-block:: none

  src
  |__application
     |____________config
     |____________sox
     |____________task
     |____________wscript
  |__engine
     |____________bmsctrl
     |____________config
     |____________database
     |____________diag
     |____________sysctrl
     |____________task
     |____________wscript
  |__general
     |____________config
     |____________includes
     |____________main.c
     |____________main.h
     |____________nvic.c
     |____________nvic.h
     |____________version.c
     |____________version.h
     |____________wscript
  |__hal
  |__module
     |____________adc
     |____________can
     |____________cansignal
     |____________chksum
     |____________com
     |____________config
     |____________contactor
     |____________dma
     |____________eeprom
     |____________intermcu
     |____________io
     |____________isoguard
     |____________ltc
     |____________mcu
     |____________rcc
     |____________rtc
     |____________spi
     |____________timer
     |____________uart
     |____________utils
     |____________watchdog
     |____________wscript
  |__os
     |____________FreeRTOS
     |____________os.c
     |____________os.h
     |____________wscript
  |____________doxygen.h
  |____________STM32F429ZIT6_FLASH.ld
  |__wscript