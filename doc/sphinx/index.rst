======
foxBMS
======

.. include:: ./macros.rst

Welcome to `foxBMS <http://www.foxbms.org/>`_. This is the documentation of the free, open, flexible battery management system.

.. note::
   
      **The foxBMS hardware and software is currently in beta test phase. The beta version of foxBMS is not designed to be used
      as-is in a vehicle on the road or in a production environment or something similar: it is currently for evaluation only.**

First, general information about foxBMS can be found (:ref:`infos`): who made it, its purpose and the intended audience.

Then the getting started guide (:ref:`start`) allows starting foxBMS rapidly:

 - The location of layout and schematic files in the HTML documentation is indicated
 - The hardware guide describes how the hardware is structured and works
 - The software guide explains how to configure and flash the software
 - The CAN documentation shows how to communicate with the system via the CAN bus if needed

To study and modify the software, the software documentation (:ref:`sw`) then explains the structure of the software and of the most import modules, how the basic tasks run and how to call
user-defined functions easily. A FAQ answers the most common questions. A detailed description of functions and variables can be found in the Doxygen documentation.

Finally, the hardware documentation (:ref:`hw`) gives the details needed to understand the foxBMS hardware. 

.. _infos:

.. toctree::   
   :titlesonly:
   :numbered:
   :caption: General information about foxBMS
   
   general/team/team
   general/motivation/motivation
   general/license/license
   general/link_hw_sw/link_hw_sw

.. _start:

.. toctree::   
   :titlesonly:
   :numbered:
   :caption: Getting started with foxBMS

   gettingstarted/quickstart_general
   gettingstarted/quickstart_SW
   gettingstarted/quickstart_HW_flash
   gettingstarted/quickstart_HW_cables
   gettingstarted/quickstart_HWspec
   gettingstarted/can_com
   gettingstarted/quickstart_HW_SW-check

.. _sw:

.. toctree::  
   :titlesonly:  
   :numbered:
   :maxdepth: 1
   :caption: Software documentation
   
   software/file_structure/file_structure
   software/components/components
   software/basics/basics
   software/toolchain/toolchain
   software/modules/modules
   software/advanced_topics/advanced_topics
   software/faq/faq
   software/tools/tools

.. _hw:

.. toctree:: 
   :titlesonly:
   :numbered:
   :caption: Hardware documentation

   hardware/basics/basics
   hardware/schematics/schematics
   hardware/additional_hw/additional_hw
   hardware/bjb/bjb
   hardware/toolchain/toolchain
   
   
.. ignore
   qstart



