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

void download(char * dir, char * url);