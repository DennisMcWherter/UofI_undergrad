EXTRA_CFLAGS +=
KERNEL_SRC:= /lib/modules/$(shell uname -r)/build
SUBDIR= $(PWD)
GCC:=gcc
RM:=rm

.PHONY : clean

obj-m:= scheduler.o

all: clean modules factorial

modules:
	$(MAKE) -C $(KERNEL_SRC) M=$(SUBDIR) modules

factorial:
	$(GCC) -o factorial -O0 -ggdb factorial.c

clean:
	$(RM) -f factorial *.ko *.o *.mod.c Module.symvers
