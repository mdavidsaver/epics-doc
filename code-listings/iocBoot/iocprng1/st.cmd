#!../../bin/linux-x86/prng

cd "../.."

## Register all support components
dbLoadDatabase "dbd/prng.dbd"
prng_registerRecordDeviceDriver pdbbase

## Load record instances
dbLoadRecords("db/prng.db","P=test:prng,D=Random,S=324235")
dbLoadRecords("db/prng.db","P=test:prngasync,D=Random Async,S=324235")
dbLoadRecords("db/prng.db","P=test:prngintr,D=Random Intr,SCAN=I/O Intr,S=324235")
dbLoadRecords("db/prng.db","P=test:prngrate,D=Random Intr Rate,SCAN=I/O Intr,S=324235,TPRO=1")

cd ${TOP}/iocBoot/${IOC}
iocInit
