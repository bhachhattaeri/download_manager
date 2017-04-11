
#include <unistd.h>
#include <string.h>
#include <magic.h>
#include <stdio.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <pthread.h>


void * download(void * obj);

void download_wrapper(char * dir[], char * url[], char** times);
void check_for_updates(char * url, long prev_mod, char * download_dir);

//int main();

