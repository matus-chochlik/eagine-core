=============================
Enabling and using core-dumps
=============================

Enabling core-dumps
-------------------

In order to enable the creation of `.crash` files for non-packaged local
executables, add the following to the `~/.config/apport/settings` file:

::
  
  [main]
  unpackaged=true


Unpacking a crash file
----------------------

When the system reports that a `.crash` file has been created (typically in the
`/var/crash/` directory), use the following command to process and unpack the
information into a new or existing empty directory:

::

  apport-unpack /var/crash/file.crash /path/to/output/directory/


Viewing the unpacked content of a crash file
--------------------------------------------

To list the files unpacked from the crash file do:

::

  ls -la /path/to/output/directory/

To view the stack trace unpacked from the `.crash` file do:

::

  cat /path/to/output/directory/Stacktrace


Opening core-dump in `gdb`
--------------------------

In order to open a core-dump file in `gdb` for some crashed executable do:

::

  gdb /path/to/crashed/executable -c /path/to/output/directory/CoreDump
