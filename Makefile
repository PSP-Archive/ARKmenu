TARGET = ARKMENU
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)
INCDIR = include
LIBDIR = libs
LIBS = -lstdc++ -lpspgu -lpspgum -lpng -lz -lm -lpspinit -lpsploadexec_kernel -lpspsystemctrl_user
LDFLAGS =
EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = arkMenu

OBJS = \
		main.o \
		graphics/font.o \
		graphics/framebuffer.o \
		graphics/graphics.o \
		graphics/libccc.o \
		graphics/intraFont.o \
		src/debug.o \
		src/common.o \
		src/gamemgr.o \
		src/entry.o \
		src/iso.o \
		src/cso.o \
		src/eboot.o \
		src/menu.o \
		src/controller.o


PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
