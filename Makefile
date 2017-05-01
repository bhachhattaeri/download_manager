all: daemon userinput

daemon: daemon.o download_manager.o
	gcc daemon.o download_manager.o -o daemon -lcurl

download_manager.o: download_manager.c download_manager.h
	gcc -c download_manager.c -pthread

daemon.o: daemon.c
	gcc -c daemon.c

userinput: userinput.o download_manager.o
	gcc userinput.o download_manager.o -o userinput -lcurl

userinput.o: userinput.c
	gcc -c userinput.c
clean:
	rm *o hello
