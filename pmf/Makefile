TARGET = PMF
CFLAGS = -O2 -G0 -Wall
#CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)
INCDIR = include
LIBDIR = libs
LIBS = -lpspgu -lpspgum -lpspmpeg -lpspaudio -lpsppower -lpng -lz -lm
LDFLAGS = -L libs
EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = pmf test
PSP_FW_VERSION=371

OBJS = \
		main.o \
		pmf_audio.o \
		pmf_decoder.o \
		pmf_reader.o \
		pmf_video.o


PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
