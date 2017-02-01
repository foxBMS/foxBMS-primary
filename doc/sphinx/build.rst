Build
=====

Requirements
------------

The build process of *foxBMS* heavily depends on Python, for example, for
code generation purposes. *foxBMS* hence comes with its own Python
distribution, called *foxConda*, powered by Anaconda (http://continuum.io).
These build instructions assume that *foxConda* was successfully installed
and that the ``PATH`` environment has been adjusted
accordingly. For further information refer to the *foxConda* documentation.

Obtaining the sources
---------------------

FIXME GitHub instruction are to follow


Build
-----

python tools/waf-1.8.12 configure --check-c-compiler=gcc build
