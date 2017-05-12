.. include:: ../../macros.rst

Hardware Toolchain
==================

Layout and schematics
---------------------

The layout and schematics were done with Cadsoft Eagle version 6.5.0 which is now owned by `AUTODESK <https://www.autodesk.com/products/eagle/overview>`_.


Information on Debugging
------------------------

Two types of debugger have been tested with |foxBMS|.

* The first one is the `Segger J-Link Plus <https://www.segger.com/j-link-plus.html>`_ with the `adapter for 19-Pin Cortex-M <https://www.segger.com/jlink-adapters.html#CM_19pin>`_ (the adpter is needed to connect the debugger to the |master|). A cheaper debugger solution is the `Segger J-Link Base <https://www.segger.com/jlink_base.html>`_ which also needs the adapter.

* The second type of debugger is the `Lauterbach Debugger <http://www.lauterbach.com>`_ µTrace for Cortex-M (see the “Products” section in the navigation bar on the left). The Lauterbach debugger provides more debugging functionalities, but is also more expensive than the J-Link.

