
NAME		= datapath

CC		= gcc

LD              = $(CC)

INCLUDE         = -I./include\
			      -I/usr/include/libusb-1.0

DEBUG           += -g -DVERBOSE

CFLAGS          = $(INCLUDE) -c $(DEBUG) -Wall -O3

LDFLAGS         = -lm -lusb-1.0

###### SOURCES  #################

CSRCS		= datapath-host.c dp-bulkusb.c serial-port.c parse-args.c helpers.c

COBJS           = $(CSRCS:.c=.o)

PROGS           = datapath-host dp-bulkusb

.PHONY: rebuild clean

all:  $(PROGS) Makefile

$(COBJS): $(wildcard ./include/*.h)

datapath-host: datapath-host.o serial-port.o parse-args.o helpers.o
	$(LD) $(LDFLAGS) -o $@ $^

dp-bulkusb: dp-bulkusb.o parse-args.o helpers.o
	$(LD) $(LDFLAGS) -o $@ $^

clean:
	$(RM) $(PROGS) $(COBJS) \
	lib$(NAME).a *.report\
        *.lst *~ ./include/*~ a.out 

rebuild: rebuild 
	@$(MAKE) -s clean
	@$(MAKE) -s


