.. _checksum_tool:

Checksum Tool
=============

.. include:: ../../../macros.rst

.. highlight:: python

This is the documentation of the **build process** and **checksum integration into that process** of the checksum feature in |foxBMS|.
For the setup (enabling/disabling) of the checksum verification on the microcontroller see :ref:`checksum_module`.

.. contents:: Table Of Contents

Module Files
~~~~~~~~~~~~

Source:

 - tools\\checksum\\chksum.py

Configuration:

 - tools\\checksum\\chksum.ini

Dependencies
~~~~~~~~~~~~
Python modules used:
 - sys
 - os
 - ConfigParser
 - Bits
 - binascii
 - numpy
 - struct
 - argparse

These modules usually are included in a `foxconda` python environment.

Procedure
~~~~~~~~~

Checksum is an after build prcoess.

.. code:: bash

    python tools\waf-1.8.12 configure --check-c-compiler=gcc
    python tools\waf-1.8.12 build
    python tools\waf-1.8.12 chksum

How does it work
~~~~~~~~~~~~~~~~

#. The step

    .. code:: bash
    
        python tools\waf-1.8.12 configure --check-c-compiler=gcc
    
    configures waf as needed to build.
    
#. The step

    .. code:: bash
    
        python tools\waf-1.8.12 build
    
    builds the files
    
     - ``foxbms.elf``
     - ``foxbms.hex``
     - ``foxbms_flash.bin``
     - ``foxbms_flashheader.bin``
#. The step

    .. code:: bash
    
        python tools\waf-1.8.12 chksum
    
    calls the waf function ``cksum(conf)``.
    
        #. At first read the ``foxbms.hex`` file and calculates the checksum. The checksum is written back into the ``foxbms.hex`` file by the checksum script.
        #. At next the gbd debugger is called and replaces the initial ``ver_sw_validation.Checksum_u32`` in ``foxbms.elf`` with the correct checksum.
        #. After that the ``objcopy`` is called to regenerate the ``foxbms_flashheader.bin`` of ``foxbms.elf``.

Related Modules
~~~~~~~~~~~~~~~
The checksum tool is related on the ``Checksum Module`` configuration (see :ref:`checksum_module`).