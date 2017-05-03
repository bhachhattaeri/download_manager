all: daemon userinput gui

daemon: daemon.o download_manager.o
	gcc daemon.o download_manager.o -o daemon -lcurl -pthread

download_manager.o: download_manager.c download_manager.h
	gcc -c download_manager.c -pthread

daemon.o: daemon.c
	gcc -c daemon.c

gui: gui.c userinput.c
	gcc `pkg-config --cflags gtk+-3.0` -o gui gui.c userinput.c `pkg-config --libs gtk+-3.0` -pthread

userinput: userinputMain.c userinput.c userinput.h
	gcc -o userinput userinputMain.c userinput.c userinput.h

clean:
	rm *o hello

install: startup.sh
	cp ./daemon /usr/bin/DownloadManager
	cp startup.sh /etc/init.d/DownloadManager
	chmod +x /etc/init.d/DownloadManager
	chown root:root /etc/init.d/DownloadManager
	update-rc.d DownloadManager defaults
