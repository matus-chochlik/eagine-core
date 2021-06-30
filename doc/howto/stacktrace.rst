===========================
Stacktraces with assertions
===========================

This document describes how to enable stack-traces with the ``EAGINE_ASSERT``
macros. For the full functionality the ``g++`` compiler is required.

Required Debian packages
------------------------

Install the following packages (where N is your current major g++ version):

::

  sudo apt install libgcc-N-dev libboost-stacktrace-dev
