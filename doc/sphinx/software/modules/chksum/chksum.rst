.. _checksum_module:

Checksum Module
===============

.. include:: ../../../macros.rst

.. highlight:: C

This is the documentation of the source files  the checksum feature in |foxBMS|.
For the toolchain/buildprocess of the checksum feature see :ref:`checksum_tool`.

The chksum module provides the capability to verify data integrity during runtime 
for multiple purposes, e.g. firmware validation and verification of sent/received data.

.. contents:: Table Of Contents

Module Files
~~~~~~~~~~~~

Source:
 - src\\module\\chksum\\chksum.h
 - src\\module\\chksum\\chksum.c


Checksum Module Description
~~~~~~~~~~~~~~~~~~~~~~~~~~~

The chksum module offers 2 possibilities to check data integrity, Modulo32BitAddition and 
a hardware based, slightly modified CRC32 implementation. The following short examples
demonstrate usage of this module:

.. code-block:: c

    uint32_t chksum = CHK_crc32((uint8_t*)0x08000000, 0x1000) -> hashes 4kB of code, starting at 0x08000000 using CRC32 algorithm

    uint32_t chksum = CHK_modulo32addition((uint8_t*)0x08000000, 0x1000) -> hashes 4kB of code, starting at 0x08000000 using Modulo32BitAddition algorithm


During startup CHK_crc32 function is used to verify integrity of the flashed firmware image, after calculation the crc32 chksum is compared to a hardoded
value within the flashheader struct. When both values match the firmware is valid and execution continues else an error will be reported. 

The flashheader contains the validation and checksum information with memory areas of separate SW partitions, it gets generated during compilation by an external chksum tool, written in python.
Furthermore it also provides the possibility for software versioning.  


Checksum Configuration
~~~~~~~~~~~~~~~~~~~~~~
Enabling/disabling of the checksum verification at startup is set by the following macro. The macro is foxygen configurable.

.. code-block:: C

  /*fox
   * enables checking of flash checksum at startup.
   * @var enable flash checksum
   * @level advanced
   * @type select(2)
   * @default 0
   * @group GENERAL
   */
  #define BUILD_MODULE_ENABLE_FLASHCHECKSUM           1
  //#define BUILD_MODULE_ENABLE_FLASHCHECKSUM           0

Checksum Default Configuration
------------------------------

The default value enables checksum verification at startup.

Related Modules
~~~~~~~~~~~~~~~
The checksum tool is related on the ``Checksum Tool`` configuration (see :ref:`checksum_tool`).