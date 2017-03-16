<<<<<<< HEAD
void download(char* div, char url);
=======
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
>>>>>>> 214e40867cafa0c755663050df0514f5592c7b53
