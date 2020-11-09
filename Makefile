AS      = $(CROSS_BLD)as
LD      = $(CROSS_BLD)ld
CC      = $(CROSS_BLD)gcc
CPP     = $(CC) -E
CXX     = $(CROSS_BLD)c++
AR      = $(CROSS_BLD)ar
NM      = $(CROSS_BLD)nm
LDR     = $(CROSS_BLD)ldr
STRIP   = $(CROSS_BLD)strip
OBJCOPY = $(CROSS_BLD)objcopy
OBJDUMP = $(CROSS_BLD)objdump
RANLIB  = $(CROSS_BLD)ranlib

TARGET  ?= $(TAR)

ifeq (${TARGET},hwrtos)
   CROSS_BLD  = arm32-linux-gnueabi-
   CROSS_HOST = arm32-linux-gnueabi
else
   CROSS_BLD  =
   CROSS_HOST =
endif

prom = digesttool
deps = dealcmdpara.h sm3.h
obj = main.o dealcmdpara.o sm3.o

$(prom): $(obj)
	$(CC) -o $(prom) $(obj)
	$(STRIP) $(prom)
%.o: %.c $(deps)
	$(CC) -c $< -o $@
clean:
	rm -rf $(obj) $(prom)
