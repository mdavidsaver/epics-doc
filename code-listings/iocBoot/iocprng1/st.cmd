#!../../bin/linux-x86/prng

## You may have to change prng to something else
## everywhere it appears in this file

< envPaths

cd ${TOP}

## Register all support components
dbLoadDatabase "dbd/prng.dbd"
prng_registerRecordDeviceDriver pdbbase

## Load record instances
dbLoadRecords("db/prng.db","P=test:prng,S=324235")
dbLoadRecords("db/prngasync.db","P=test:prngasync,S=324235")

cd ${TOP}/iocBoot/${IOC}
iocInit

## Start any sequence programs
#seq sncxxx,"user=mdavidsaverHost"
