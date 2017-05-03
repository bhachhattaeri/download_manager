#include "userinput.h"

int main(int argc, char** argv) {
  notGui = 1;
  if(argc <= 1) {
    print_help();
    exit(1);
  }
   //   printf("Here\n" );

  initialize();
     // printf("Here\n" );

  int counter = 1;

  while(counter < argc) {
    if(strcmp(argv[counter], "-c")==0) { // if custom
      custom = 1;
      if(setOptions(1)==-1) { //error finding config file
        exit(1);
      }
      break;
    }
    counter++;
  }

  counter = 1;
  while(counter < argc) {
    if(argv[counter][0]=='-') { //if is flag
      //printf("%s\n",argv[counter] );
      run_program(argv[counter], argv[counter+1]);
    }
    counter++;
  }

  return 0;
} 


