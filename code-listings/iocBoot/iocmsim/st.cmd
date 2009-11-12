#!../../bin/linux-x86/msim

## You may have to change msim to something else
## everywhere it appears in this file

< envPaths

cd("${TOP}")

## Register all support components
dbLoadDatabase("dbd/msim.dbd")
msim_registerRecordDeviceDriver(pdbbase)

addmsim(0, -1000, 1000, 2.0)

dbLoadRecords("db/motor.db","P=test:,M=msim")

cd(${TOP}/iocBoot/${IOC})
iocInit()
