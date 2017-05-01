all: daemon gui

daemon: daemon.o download_manager.o
	gcc daemon.o download_manager.o -o daemon -lcurl -pthread

download_manager.o: download_manager.c download_manager.h
	gcc -c download_manager.c -pthread

daemon.o: daemon.c
	gcc -c daemon.c

gui: gui.c userinput.c
	gcc `pkg-config --cflags gtk+-3.0` -o gui gui.c userinput.c `pkg-config --libs gtk+-3.0`

clean:
	rm *o hello
