TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build the IOC application

PROD_IOC = prng
# prng.dbd will be created and installed
DBD += prng.dbd

# prng.dbd will be made up from these files:
prng_DBD += base.dbd

prng_DBD += prngdev.dbd
prng_DBD += prngdist.dbd

# Add all the support libraries needed by this IOC
#prng_LIBS += xxx

# prng_registerRecordDeviceDriver.cpp derives from prng.dbd
prng_SRCS += prng_registerRecordDeviceDriver.cpp
prng_SRCS += devprng.c
prng_SRCS += devprngasync.c
prng_SRCS += devprngintr.c
prng_SRCS += devprngintrrate.c

prng_SRCS += devprngdist.c
prng_SRCS += iocshdist.c
prng_SRCS += drvprngunif.c
prng_SRCS += drvprnggaus.c

# Build the main IOC entry point on workstation OSs.
prng_SRCS_DEFAULT += prngMain.cpp
prng_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#prng_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Finally link to the EPICS Base libraries
prng_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

