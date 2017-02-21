#include "userinput.h"

int main(int argc, char** argv) {

  if(argc <= 1) {
    print_help();
    exit(1);
  }
  
  int counter = 1;
  while((counter+1) < argc) {
    run_program(argv[counter], argv[counter+1]);
    counter += 2;
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
  assert(argument);
  if(strcmp(flag, "-u")==0) {
    update();
  } else if(strcmp(flag, "-d")) {
    download(argument);
  } else if(strcmp(flag, "-f")) {
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
}

void print_help() {
  printf("~~~~~~~~~~~~~~~~~~~~~\n");
  printf("Program help:\n");
  printf("~~~~~~~~~~~~~~~~~~~~~\n");
  printf("-u: Runs an update method for stored urls.\n");
  printf("-d <url>: Downloads from url.\n");
  printf("-f <filename>: Downloads from urls in file.\n");
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
