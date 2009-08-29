#!../../bin/linux-x86/prng

< envPaths

cd ${TOP}

dbLoadDatabase "dbd/prng.dbd"
prng_registerRecordDeviceDriver pdbbase

createPrng(0,34342,"Uniform")
createPrng(1,34342,"Gaussian")

dbLoadRecords("db/prng.db","P=prng:unif,D=Random Distribution,S=#C0 S0 @")
dbLoadRecords("db/prng.db","P=prng:gaus,D=Random Distribution,S=#C1 S0 @")

cd ${TOP}/iocBoot/${IOC}
iocInit
