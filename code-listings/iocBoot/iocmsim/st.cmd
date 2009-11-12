#!../../bin/linux-x86/msim

## You may have to change msim to something else
## everywhere it appears in this file

< envPaths

cd("${TOP}")

## Register all support components
dbLoadDatabase("dbd/msim.dbd")
msim_registerRecordDeviceDriver(pdbbase)

dbLoadRecords("db/motor.db","P=test:,M=msim")

cd(${TOP}/iocBoot/${IOC})
iocInit()
