

void download(char * dir, char * url){
	CURL * c;
 	curl_global_init(CURL_GLOBAL_DEFAULT);		//sets up the required environment variables
  	c = curl_easy_init();		
  	FILE * f = fopen(dir, "w+");
   	if(c){
   		curl_easy_setopt(c, CURLOPT_URL, url);			// uses the url to retrieve info
   		curl_easy_setopt(c, CURLOPT_VERBOSE, 1);		// provides meaningful message to the user
   		curl_easy_setopt(c, CURLOPT_WRITEDATA, f);		// stored the retrieved file in that location
   		curl_easy_perform(c);

		curl_easy_cleanup(c);
	}else perror(stderr,"Oops...Something went wrong. Please try again.\n");
	fclose(f);
}
<<<<<<< HEAD
int main(int argc, char *argv[]){
	download(argv[1],argv[2]);
	/*
	int fd = open("log2.txt",O_CREAT | O_TRUNC | O_RDWR, S_IRUSR |  S_IWUSR);
	curl_global_init(CURL_GLOBAL_ALL);
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

	}*/
	  return 0;
}
=======
>>>>>>> 214e40867cafa0c755663050df0514f5592c7b53
