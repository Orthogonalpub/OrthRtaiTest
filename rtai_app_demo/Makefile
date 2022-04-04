TARGET = periodic_task  

SRCS = periodic_task.c 

prefix := $(shell rtai-config --prefix)

ifeq ($(prefix),)
$(error Please add <rtai-install>/bin to your PATH variable)
endif

OBJECTS = $(SRCS:.c=.o)

CC = $(shell rtai-config --cc)

LXRT_CFLAGS = $(shell rtai-config --lxrt-cflags)

LXRT_LDFLAGS = $(shell rtai-config --lxrt-ldflags)


all: $(TARGET)

%.o: %.c
	$(CC) -c  $(LXRT_CFLAGS) $<

$(TARGET) :  $(OBJECTS)
	$(CC)  -o $(TARGET) $(OBJECTS) $(LXRT_LDFLAGS) -llxrt 

clean:
	rm -f *.o  *~ core.* $(TARGET)
	.PHONY: clean
