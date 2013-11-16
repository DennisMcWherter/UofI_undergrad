EXTRA_CFLAGS +=
KERNEL_SRC:= /lib/modules/$(shell uname -r)/build
SUBDIR= $(PWD)
GCC:=gcc
RM:=rm

.PHONY : clean

all: clean modules factorial

obj-m:= timer_module.o

modules:
	$(MAKE) -C $(KERNEL_SRC) M=$(SUBDIR) modules

factorial:
	$(GCC) -o factorial -O0 -ggdb factorial.c

clean:
	$(RM) -f factorial *.ko *.o *.mod.c Module.symvers
