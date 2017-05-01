OBJS = daemon.o user_input.o
CC = gcc
LFLAGS = -lcurl

daemon : daemon.o download_manager.o
	$(CC) $(LFLAGS) daemon.o download_manager.o -o daemon

daemon.o : daemon.c download_manager.h
	$(CC) $(CFLAGS) daemon.c

user_input.o : userinput.h userinput.c download_manager.h
	$(CC) $(CFLAGS) userinput.c

userinput : user_input.o
	$(CC) $(LFLAGS) -o userinput user_input.o  download_manager.o

download_manager.o : download_manager.h download_manager.c
	$(CC) $(CFLAGS) download_manager.c

clean:
	\rm *.o *~ p1