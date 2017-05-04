#include "userinput.h"

#define FILE_TYPES (3)
#define INITIAL_URL_SIZE (4)

int custom = 0;
static char** fileTypes;
static char** directories;
static char** urls;
static char** lastModified;
static char** lastStored;
static int numberOfUrls = 0;
static int maximumNumberOfUrls = INITIAL_URL_SIZE;
static int sockfd;
static int isSocketInitialized = 0;
static struct sockaddr_in servaddr;
int notGui = 0;

// uncomment to run without gui
void initialize() {
  fileTypes = malloc(sizeof(char*) * (FILE_TYPES + 1));
  directories = malloc(sizeof(char*) * (FILE_TYPES + 1));
  
  char* path = getenv("HOME");  

  fileTypes[0] = "jpg";
  char* zero = strdup(path);
  zero = realloc(zero, strlen(path)+19);
  strcpy(zero, path); //"Downloads/Images/";
  strcat(zero, "/Downloads/Images/");
  directories[0] = zero;

  fileTypes[1] = "mov";
  char* one = strdup(path);
  one = realloc(one, strlen(path)+19);
  strcpy(one, path); //"Downloads/Images/";
  strcat(one, "/Downloads/Movies/");
  directories[1] = one;

  fileTypes[2] = "doc";
  char* two = strdup(path);
  two = realloc(two, strlen(path)+22);
  strcpy(two, path);
  strcat(two, "/Downloads/Documents/");
  directories[2] = two;

  fileTypes[3] = "default";
  char* three = strdup(path);
  three = realloc(three, strlen(path)+20);
  strcpy(three, path); //"Downloads/Images/";
  strcat(three, "/Desktop/");
  directories[3] = three; 

  urls = malloc(sizeof(char*) * INITIAL_URL_SIZE);
  lastStored = malloc(sizeof(char*) * INITIAL_URL_SIZE);
  lastModified = malloc(sizeof(char*) * INITIAL_URL_SIZE);
}

int setOptions(int customDestinations) {
  FILE* file = NULL;
  
  file = fopen("options.conf", "r");
  if(file == NULL) {
    err_opening_config_file();
    return -1;
  }

  char* line = NULL;
  size_t len;

  int category = 0;
  while(getline(&line, &len, file) != -1) {
    cleanLine(line);
    if(strlen(line) == 0) {
      continue;
    }

    if(checkHeading(line, &category)) {
      continue;
    }

    if(category == 1 && customDestinations == 1) {
      storeCustomDestinations(line);
    } else if(category == 2) {
      loadURL(line);
    }
  }

  fclose(file);
  return 0;
}

void storeCustomDestinations(char* line) {
  if(line[0] != '.' || strstr(line, "=")!=0) {
    return;
  }

  char* copy = strdup(line);
  char* fileExtension = strtok(copy, "=") + 1;
  char* path = strtok(NULL, "=");

  for(int i = 0; i < FILE_TYPES; i++) {
    if(strcmp(fileExtension,fileTypes[i])==0) {
      directories[i] = path;
    }
  }
}

void loadURL(char* line) {
  if(numberOfUrls < maximumNumberOfUrls) {
    char* copy = strdup(line);
    urls[numberOfUrls] = strtok(copy, "|");
    cleanLine(urls[numberOfUrls]);
    lastStored[numberOfUrls] = strtok(NULL, "|");
    cleanLine(urls[numberOfUrls]);
    lastModified[numberOfUrls] = strtok(NULL, "|");
    cleanLine(urls[numberOfUrls]);
    numberOfUrls++;
  } else {
    maximumNumberOfUrls *= 2;
    urls = realloc(urls, maximumNumberOfUrls * sizeof(char*));
    lastStored = realloc(lastStored, maximumNumberOfUrls * sizeof(char*));
    lastModified = realloc(lastModified, maximumNumberOfUrls * sizeof(char*));
   
    char* copy = strdup(line);
    urls[numberOfUrls] = strtok(copy, "|");
    cleanLine(urls[numberOfUrls]);
    lastStored[numberOfUrls] = strtok(NULL, "|");
    cleanLine(urls[numberOfUrls]);
    lastModified[numberOfUrls] = strtok(NULL, "|");
    cleanLine(urls[numberOfUrls]);
    numberOfUrls++;
  }
}

int checkHeading(char* line, int* category) {
  if(strcmp(line, "#CUSTOM") == 0) {
    *category = 1;
    return 1;
  } else if(strcmp(line, "#URLS")==0) {
    *category = 2;
    return 1;
  }
  return 0;
}

void rewriteLines() {
  FILE* file = fopen("options.conf", "r");
  if(file == NULL) {
    return;
  }

  FILE* temporary = fopen("temporary.conf", "w+");

  char* line = NULL; 
  size_t len;

  while(getline(&line, &len, file) != -1) {
    cleanLine(line);
    int found = 0;

    for(int i = 0; i < numberOfUrls; i++) {
      char* url = urls[i];
      char* dir = lastStored[i];
      char* time = lastModified[i];
 
      char oldLine[200];
      memset(oldLine, 0, 200);
      strcpy(oldLine, url);
      strcat(oldLine, "|");
      strcat(oldLine, dir);
      strcat(oldLine, "|");
      strcat(oldLine, time);
      if(strcmp(line, oldLine)==0) {    
        found = 1;
        break;
      }
    }
    if(found==0) {
      fprintf(temporary, "%s\n", line);
    }
  }
  fclose(temporary);
  fclose(file);

  file =fopen("options.conf", "w+");
  temporary = fopen("temporary.conf", "r");

  while(getline(&line, &len, temporary) != -1) {
    fprintf(file, "%s", line);
  }

  fclose(temporary);
  fclose(file);

  remove("temporary.conf");
  
}

void update() {
  printf("update %d", numberOfUrls);
  for(int i = 0; i < numberOfUrls; i++) {
    char* url = urls[i];
    char* dir = lastStored[i];
    char* time = lastModified[i];
    callDaemonToDownload(url, dir, time);
  }
  rewriteLines();
  for(int i = 0; i < numberOfUrls; i++) {
    char* url = urls[i];
    char* dir = lastStored[i];
    saveToConfigFile(url, dir);
  }
}

void initializeSocket() {
  printf("initialize Socket\n");
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd == -1) {
    perror("socket()");
    printf("error\n");
  }
  bzero(&servaddr, sizeof(servaddr));

  servaddr.sin_family = AF_INET;
  servaddr.sin_port=htons(1234);

  inet_pton(AF_INET, "localhost", &(servaddr.sin_addr));

  if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))== -1){
    perror("connect()");
    printf("error\n");
  }

  isSocketInitialized = 1;
}

void callDaemonToDownload(char* url, char* dir, char* time) {
  //if(!isSocketInitialized) {
    initializeSocket();
  //}
  printf("Inside calling daemon to download\n");

           // return;
  char sendLine[200];
  char recvLine[200];

  bzero(sendLine, 200);
  bzero(recvLine, 200);

  strcpy(sendLine, "1");
  strcat(sendLine, "\t");
  strcat(sendLine, url);
  strcat(sendLine, "\n");
  strcat(sendLine, dir);
  strcat(sendLine, "\n");
  strcat(sendLine, time);
  strcat(sendLine, "\n");
  printf("Line: %s\n",sendLine );
  write(sockfd, sendLine, strlen(sendLine)+1);

  shutdown(sockfd, SHUT_WR);
  if(notGui) {
    double buffer;
    int ret = read(sockfd, (char*)&buffer, sizeof(double));
    printf("Time taken: %lf\n", buffer);
    printf("Completed\n");
  }
  close(sockfd);
}

void saveToConfigFile(char* url, char* dir) {
  FILE* file = NULL;

  file = fopen("options.conf", "a");
  if(file == NULL) {
    err_opening_config_file();
    return;
  }

  if(dir == NULL) {
    dir = getDirectoryFromUrl(url);
  }

  char line[300];
  memset(line, 0, 300);
  strcpy(line, url);
  strcat(line, "|");
  strcat(line, dir);
  strcat(line, "|");

  sprintf(line+strlen(line), "%lu\n", (unsigned long)time(NULL));

  fprintf(file, "%s", line);
  fclose(file);
}

void run_program(char* flag, char* argument) {
  assert(flag);
  if(strcmp(flag, "-u")==0) {
    if(custom == 0) {
      setOptions(0);
    }
    update();
  } else if(strcmp(flag, "-d")==0) {
    if(argument == NULL) {
      err_downloading_from_url();
      return;
    }
    char* dir = getDirectoryFromUrl(argument);
    callDaemonToDownload(argument, dir, "");

  } else if(strcmp(flag, "-f")==0) {
    if(argument == NULL) {
      err_file_not_found();
      return;
    }
    downloadFromFile(argument);
  } else if(strcmp(flag, "-ds")==0) {
    if(argument == NULL) {
      err_downloading_from_url();
      return;
    }
    char* dir = getDirectoryFromUrl(argument);
    callDaemonToDownload(argument, dir, "");
    saveToConfigFile(argument, dir);
  }
}

void cleanLine(char* line) {
  // removes trailing newline character
  if(line[strlen(line)-1]=='\n') {
    line[strlen(line)-1] = '\0';
  }

  // removes trailing whitespace
  int counter = strlen(line) - 1;
  while(counter >= 0) {
    if(isspace(line[counter])) {
      line[counter] = '\0';
      counter--;
    } else {
      break;
    }
  }
}

char* getDirectoryFromUrl(char* url) {
  char* fileExtensionLocation = strrchr(url, '.');  
  if(fileExtensionLocation == NULL) {
    return directories[FILE_TYPES]; // default
  }

  fileExtensionLocation++;
  for(int i = 0; i < FILE_TYPES; i++) {
    if(strcmp(fileExtensionLocation, fileTypes[i])==0) {
      return directories[i];
    }
  }
  return directories[FILE_TYPES];
}

void downloadFromFile(char* fileName) {
  FILE* file = NULL;
  file = fopen(fileName, "r");
  if(file == NULL) {
    err_file_not_found();
    return;    
  }

  char* line = NULL;
  size_t len = 0;

  while(getline(&line, &len, file) != -1) {
    if(line) {
      cleanLine(line);
      char* dir = getDirectoryFromUrl(line);
      callDaemonToDownload(line, dir, "");
    }
  }
  fclose(file);
}

void print_help() {
  printf("~~~~~~~~~~~~~~~~~~~~~\n");
  printf("Program help:\n");
  printf("~~~~~~~~~~~~~~~~~~~~~\n");
  printf("-u: Runs an update method for stored urls.\n");
  printf("-d <url>: Downloads from url.\n");
  printf("-f <filename>: Downloads from urls in file.\n");
}

void err_opening_config_file() {
  printf("Error opening config file. Terminating.\n");
}

void err_no_url_specified() {
  printf("No URL specified.\n");
}

void err_file_not_found() {
  printf("Input file not found.\n");
}

void err_downloading_from_url(char* url) {
  printf("Error downloading from url: %s\n", url);
}
