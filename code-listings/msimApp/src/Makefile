TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build the IOC application

PROD_IOC = msim
# msim.dbd will be created and installed
DBD += msim.dbd

# msim.dbd will be made up from these files:
msim_DBD += base.dbd
msim_DBD += motorRecord.dbd
msim_DBD += devsim.dbd

msim_SRCS += devSim.c

msim_LIBS += motor

# msim_registerRecordDeviceDriver.cpp derives from msim.dbd
msim_SRCS += msim_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
msim_SRCS_DEFAULT += msimMain.cpp
msim_SRCS_vxWorks += -nil-

msim_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

