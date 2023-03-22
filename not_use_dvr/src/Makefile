# Make sure your own directory
ROOTDIR = ../../..
include $(ROOTDIR)/Rules.make

#HOME_DIR = /home/hwjun/proj/Netra/phaseII/DVRRDK_01.06.00.11
EXEC_DIR = $(dvr_rdk_BASE)/../../bin/ti816x/open_bin

fc_PREFIX = $(dvr_rdk_BASE)/../../../ti_tools/framework_components/framework_components_3_22_02_08



LIVE_DIR = ../
#linuxutils_PATH = $(EXTERNAL_SW_ROOT)/linuxutils_3_21_00_02
EXEC = wis-streamer

all:	$(EXEC)

install:	$(EXEC)
	install $(EXEC) $(EXEC_DIR)

CC = $(CODEGEN_PREFIX)gcc
CPLUSPLUS = $(CODEGEN_PREFIX)g++

INCLUDES = -I . \
	-I$(LIVE_DIR)/BasicUsageEnvironment/include \
	-I$(LIVE_DIR)/UsageEnvironment/include \
	-I$(LIVE_DIR)/groupsock/include \
	-I$(LIVE_DIR)/liveMedia/include \
	-I$(LIVE_DIR)/netra_streamer/netra_interface

CFLAGS = $(INCLUDES) -D_LINUX -g -Wall
LIBS = $(fc_PREFIX)/fctools/packages/ti/sdo/linuxutils/cmem/lib/cmem.a470MV \
	-L$(LIVE_DIR)/BasicUsageEnvironment -lBasicUsageEnvironment \
	-L$(LIVE_DIR)/UsageEnvironment -lUsageEnvironment \
	-L$(LIVE_DIR)/groupsock -lgroupsock \
	-L$(LIVE_DIR)/liveMedia -lliveMedia

OBJS = wis-streamer.o Err.o NETRAInput.o \
	WISServerMediaSubsession.o \
	WISJPEGStreamSource.o \
	WISJPEGVideoServerMediaSubsession.o \
	WISMPEG4VideoServerMediaSubsession.o \
	WISH264VideoServerMediaSubsession.o \
	WISPCMAudioServerMediaSubsession.o \
	$(LIVE_DIR)/netra_streamer/netra_interface/Netra_interface.a

$(EXEC): $(OBJS)
	$(CPLUSPLUS) $(CFLAGS) -o $(EXEC) $(OBJS) $(LIBS)
	cp -rf $(EXEC) $(EXEC_DIR)


wis-streamer.cpp:			Err.hh

Err.cpp:				Err.hh

NETRAInput.cpp:				NETRAInput.hh Err.hh

WISServerMediaSubsession.cpp:		WISServerMediaSubsession.hh

WISServerMediaSubsession.hh:		NETRAInput.hh
WISMPEG4VideoServerMediaSubsession.hh:	WISServerMediaSubsession.hh

WISMPEG4VideoServerMediaSubsession.cpp:	WISMPEG4VideoServerMediaSubsession.hh

WISH264VideoServerMediaSubsession.hh:	WISServerMediaSubsession.hh

WISH264VideoServerMediaSubsession.cpp:	WISH264VideoServerMediaSubsession.hh

WISJPEGStreamSource.cpp:		WISJPEGStreamSource.hh
WISPCMAudioServerMediaSubsession.cpp:	WISPCMAudioServerMediaSubsession.hh


.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

.cpp.o:
	$(CPLUSPLUS) -c $(CFLAGS) $< -o $@

clean:
	rm -f *.o *~
	rm -f $(EXEC)
