.. include:: ../../macros.rst

Hardware Toolchain
==================

Layout and schematics
---------------------

The layout and schematics are done with Eagle version 6.5.0 from `CadSoft <http://www.cadsoftusa.com/>`_.


Information on debugging
------------------------



Two types of debugger have been tested with |foxBMS|.

* The first one is the `Segger J-Link Plus <https://www.segger.com/j-link-plus.html>`_ with the `adapter
  for 19-Pin Cortex-M <https://www.segger.com/jlink-adapters.html#CM_19pin>`_ (needed to connect to foxBMS)
  A cheaper solution is the `Segger J-Link Base <https://www.segger.com/jlink_base.html>`_ which needs the adapter, too.

* The second type of debugger is the `Lauterbach Debugger <http://www.lauterbach.com>`_ µTrace for Cortex-M (see the “Products”
  section in the navigation bar on the left). More debugging functionalities but more expensive than the J-Link.
