
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

void download(void * obj);

void download_wrapper(char * dir[], char * url[], int total_files);
void check_for_updates(char * url, long prev_mod, char * download_dir){

int main();

