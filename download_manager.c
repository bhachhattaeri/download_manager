#include <unistd.h>
#include <string.h>
#include <magic.h>
#include <stdio.h>
#include <sys/stat.h> 
#include <fcntl.h>
    #include <sys/types.h>
       #include <dirent.h>

#include <stdlib.h>
int main(int argc, char *argv[]){
	int fd = open("log2.txt",O_CREAT | O_TRUNC | O_RDWR, S_IRUSR |  S_IWUSR);
	int child = fork();
	if(child==0){
		char * filename = NULL;
		size_t length = 0;
		do{
			printf("Please enter the name for your file....\n");
		}while(getline(&filename,&length,stdin)<=1);
		if(filename[strlen(filename)-1]=='\n') 
			filename[strlen(filename)-1]='\0';
		char * download_dir = "/Users/Bhawesh/Desktop/Downloads";
		printf("Do you want to specify a download directory?\n");
		char c;
		scanf("%1c",&c);
		while(c!='y' &&  c!='n'){
				printf("Please specify\n" );
				c=fgetc(stdin);
		}
		if(c=='y'){
			size_t len = 0;
			download_dir=NULL;
			printf("Enter directory...");
			getline(&download_dir,&len,stdin);
			download_dir[strlen(download_dir)-1]='\0';
			while(access(download_dir,F_OK)!=0){
				puts(download_dir);
				getline(&download_dir,&len,stdin);
				download_dir[strlen(download_dir)-1]='\0';
				printf("Please enter a valid directory\n");
			}
		}
		char * full_path_name = malloc(strlen(download_dir)+strlen(filename)+1);
		full_path_name = strcat(strcat(strcat(full_path_name,download_dir),"/"),filename);
		puts(full_path_name);
		char * curl_args[]={"curl","-o",full_path_name,argv[1],NULL};
		int execret = execvp("curl",curl_args);
		if(execret<0) puts("Something went wrong...\n");
	}else{
		int status;
		waitpid(child,&status,0);

	}
	  return 0;
}