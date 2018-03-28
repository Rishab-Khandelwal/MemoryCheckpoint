all:	check

default: check

clean-ckpt:
	rm -rf myckpt

clean: clean-ckpt
	rm -rf restart hello hello.o myckpt myckpt.o libckpt.so

myckpt.o: myckpt.c
	gcc -c -Wall -fno-stack-protector -Werror -fpic -o myckpt.o myckpt.c

libckpt.so: myckpt.o
	gcc -shared -o libckpt.so myckpt.o

hello.o: hello.c
	gcc -c -Wall -Werror -fpic -o hello.o hello.c

hello:	hello.o
	gcc -g -fno-stack-protector -o hello hello.o

restart: restart.c
	gcc -g -fno-stack-protector -static -Wl,-Ttext-segment=5000000 -Wl,-Tdata=5100000 -Wl,-Tbss=5200000 -o restart restart.c

res: restart
	./restart myckpt

gdb:
	gdb --args ./restart myckpt

check: clean hello restart libckpt.so
	(sleep 3 && kill -12 `pgrep -n hello` && sleep 2 && pkill -9 -n hello) &
	LD_PRELOAD=`pwd`/libckpt.so ./hello &
	sleep 4 && make res

build:	hello restart libckpt.so


dist:	dir=`basename $$PWD`; cd ..; tar cvf $$dir.tar ./$$dir; gzip $$dir.tar
