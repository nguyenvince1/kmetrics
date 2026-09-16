obj-m += kmetrics.o

KDIR ?= /lib/modules/7.0.0-31-generic/build

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
