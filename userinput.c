#include "userinput.h"

#define FILE_TYPES (3)
#define INITIAL_URL_SIZE (4)

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

int main(int argc, char** argv) {

  if(argc <= 1) {
    print_help();
    exit(1);
  }

  initialize();

  int counter = 1;

  while(counter < argc) {
    if(strcmp(argv[counter], "-c")==0) { // if custom
      if(setOptions()==-1) { //error finding config file
        exit(1);
      }
      break;
    }
  }

  counter = 1;
  while(counter < argc) {
    if(argv[counter][0]=='-') { //if is flag
      run_program(argv[counter], argv[counter+1]);
    }
    counter++;
  }

  return 0;
}

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
  strcpy(two, path); //"Downloads/Images/";
  strcat(two, "/Downloads/Documents/");
  directories[2] = two;

  fileTypes[3] = "default";
  char* three = strdup(path);
  three = realloc(three, strlen(path)+20);
  strcpy(three, path); //"Downloads/Images/";
  strcat(three, "/Downloads/Default/");
  directories[3] = three;

  urls = malloc(sizeof(char*) * INITIAL_URL_SIZE);
  lastStored = malloc(sizeof(char*) * INITIAL_URL_SIZE);
  lastModified = malloc(sizeof(char*) * INITIAL_URL_SIZE);
}

int setOptions() {
  FILE* file = NULL;
  
  file = fopen("options.conf", "-r");
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

    if(category == 1) {
      storeCustomDestinations(line);
    } else if(category == 2) {
      loadURL(line);
      numberOfUrls++;
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
    urls = realloc(urls, maximumNumberOfUrls);
    lastStored = realloc(lastStored, maximumNumberOfUrls);
    lastModified = realloc(lastModified, maximumNumberOfUrls);
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

void update() {
  for(int i = 0; i < numberOfUrls; i++) {
    char* url = urls[i];
    char* dir = lastStored[i];
    char* time = lastModified[i];
    callDaemonToDownload(url, dir, time);
  }
}

void initializeSocket() {
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  bzero(&servaddr, sizeof(servaddr));

  servaddr.sin_family=AF_INET;
  servaddr.sin_port=htons(2017);

  inet_pton(AF_INET, "localhost", &(servaddr.sin_addr));

  connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

  isSocketInitialized = 1;
}

void callDaemonToDownload(char* url, char* dir, char* time) {
  if(!isSocketInitialized) {
    initializeSocket();
  }
  char sendLine[200];
  char recvLine[200];

  bzero(sendLine, 200);
  bzero(recvLine, 200);

  strcpy(sendLine, url);
  strcat(sendLine, "\n");
  strcat(sendLine, dir);
  strcat(sendLine, "\n");
  strcat(sendLine, time);
  strcat(sendLine, "\n");
  
  write(sockfd, sendLine, strlen(sendLine)+1);
}

void run_program(char* flag, char* argument) {
  assert(flag);
  if(strcmp(flag, "-u")==0) {
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
