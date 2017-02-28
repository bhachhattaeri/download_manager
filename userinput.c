#include "userinput.h"

#define FILE_TYPES (3)
#define INITIAL_URL_SIZE (4)

static char** fileTypes;
static char** directories;
static char** urls;
static int numberOfUrls = 0;
static int maximumNumberOfUrls = INITIAL_URL_SIZE;

int main(int argc, char** argv) {

  if(argc <= 1) {
    print_help();
    exit(1);
  }

  int counter = 1;

  while(counter < argc) {
    if(strcmp(argv[counter], "-c")==0) { // if custom
      if(setOptions()==-1) { //error finding config file
        return;
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
  
  fileTypes[0] = "jpg";
  directories[0] = "Downloads/Images/";

  fileTypes[1] = "mov";
  directories[1] = "Downloads/Movies/";

  fileTypes[2] = "doc";
  directories[2] = "Downloads/Documents/";

  fileTypes[3] = "default";
  directories[3] = "Downloads/Default/";

  url = malloc(sizeof(char*) * INITIAL_URL_SIZE);
}

int setOptions() {
  FILE* file = NULL;
  file = fopen("options.conf", "-r");
  if(file == NULL) {
    err_opening_config_file();
    return -1;
  }

  char* line = NULL;
  int len;

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
    urls[numberOfUrls] = strdup(line);
    numberOfUrls++;
  } else {
    maximumNumberOfUrls *= 2;
    urls = realloc(urls, maximumNumberOfUrls);
  }
}

int checkHeading(char* line, int* category) {
  if(strcmp(line, "#CUSTOM") == 0) {
    *category = 1;
    return 1;
  } else if(strcmp(line, "#DEFAULT") == 0) {
    *category = 2;
    return 1;
  }
  return 0;
}

void download() {
  // networking program
}

void update() {

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
    download(argument);
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
      download(line);
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
