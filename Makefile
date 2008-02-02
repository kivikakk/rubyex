
CCFLAGS = -Wall -ansi -pedantic

obj-m += rubyexa.o
rubyexa-objs := chrdev.o command.o setup.o globals.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
