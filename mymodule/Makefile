obj-m := mymodule.o
myhellomodule-objs := module

KDIR := /lib/modules/$(shell uname -r)/build        

MAKE := make

INCLUDES := -I/usr/realtime/include 

all: check default
	@echo "Done"

check: check.c
	gcc check.c  -o check

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean
