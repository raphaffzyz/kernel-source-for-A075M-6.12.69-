obj-m += nb9_driver.o

# O GitHub Actions vai injetar o caminho do kernel aqui
KDIR ?= $(PWD)/../kernel-source

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean
