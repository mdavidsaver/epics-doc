
Before building this example you must set 'EPICS_BASE' in configure/RELEASE
to point the location of EPICS Base on your system.  And you must set the
environment variable 'EPICS_HOST_ARCH' (run $EPICS_BASE/startup/EpicsHostArch).

ie

$ grep EPICS_BASE ./configure/RELEASE
EPICS_BASE=/usr/local/epics/base

or

$ grep EPICS_BASE ./configure/RELEASE
EPICS_BASE=/home/me/epics/base3.14.10


Contents

prngApp/src/

 Examples from 'Basic EPICS Device Support'

prngdev.dbd
devprng.c
devprngasync.c

 Example from 'Hardware Links and Driver Support'

prngdist.dbd
iocshdist.c
devprngdist.c
drvprngdist.h
drvprngunif.c
drvprnggaus.c
