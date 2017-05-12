.. include:: ./macros.rst

==========================
The |foxBMS| Documentation
==========================

Welcome to `foxBMS <https://www.foxbms.org/>`_. This is the documentation of the free, open and flexible battery management system from the Fraunhofer IISB. The current version of |foxbms| is |version| (more information in :ref:`fox-releases`). 

.. note::
   
      The |foxbms| hardware and software is still under development. The free and open research and development platform |foxbms| is not designed to be used as-is in road vehicles or in a production environment or any similar usage: it is for evaluation only.

First, general information about |foxbms| can be found (:ref:`infos`):

 - Information about the versions and release notes (:ref:`fox-releases`)
 - Licenses
 - Motivation
 - Overview (purpose and intended audience)
 - Roadmap
 - Who made it

Then the getting started guide (:ref:`start`) allows starting |foxbms| rapidly:

 - The location of the electronic schematic and layout files in the HTML documentation is indicated
 - The hardware guide describes how the hardware is structured and works
 - The software guide explains how to configure and flash the software
 - The CAN documentation shows how to communicate with the system via the CAN bus

To study and modify the software, the software documentation (:ref:`sw`) then explains the structure of the software and of the most import modules, how the basic tasks run and how to call user-defined functions easily. A FAQ answers the most common questions. A detailed description of the software functions and variables can be found in the |doxygen| documentation.

Finally, the hardware documentation (:ref:`hw`) gives the details needed to understand the |foxbms| hardware.

.. _infos:

.. toctree::
   :titlesonly:
   :numbered:
   :caption: General information
   
   general/release/release
   general/license/license
   general/motivation/motivation
   general/link_hw_sw/link_hw_sw
   general/roadmap/roadmap
   general/team/team

.. _start:

.. toctree::
   :titlesonly:
   :numbered:
   :caption: Getting started

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
   software/build/build

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



