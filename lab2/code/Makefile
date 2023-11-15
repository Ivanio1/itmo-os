obj-m += driver.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	sudo insmod driver.ko
	gcc -pedantic-errors -Wall -Werror -g3 -O0 --std=c99 -fsanitize=address,undefined,leak ./userapp.c -o userapp

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	sudo rmmod driver
	rm -r userapp
