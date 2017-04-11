#include "download_manager.h"


typedef struct download_obj{
	char * url;
	char * dir;
}download_obj;

/* Accepts a download_obj type, which consists of
*  an url and a download directory.
*  If download is successfully performed, returns (void*)1
*  else it will return (void*)-1 
*/
void * download(void * obj){
	download_obj * file = (download_obj*)obj;
	char * dir = file->dir;
	char * url = file->url;
	CURL * c;
 	curl_global_init(CURL_GLOBAL_DEFAULT);		//sets up the required environment variables
  c = curl_easy_init();		
  FILE * download_dir = fopen(dir, "w+");
  CURLcode success;
  int exit_succ = 0;
  if(c){
   		curl_easy_setopt(c, CURLOPT_URL, url);			// uses the url to retrieve info
   		curl_easy_setopt(c, CURLOPT_VERBOSE, 1);		// provides meaningful message to the user
   		curl_easy_setopt(c, CURLOPT_WRITEDATA, download_dir);		// stored the retrieved file in that location
   		success = curl_easy_perform(c);
   		if(success == CURLE_OK){
   			int total_time = 0;
   			success = curl_easy_getinfo(c, CURLINFO_TOTAL_TIME, total_time);		// retrieves the information
   			if(success == CURLE_OK){
   				int minutes = (total_time/60);
   				int seconds = total_time % 60;
   				printf("Total time: %d:%d\n",minutes,seconds);
   			}
   			exit_succ = 1;
   		}else{
   			perror("Sorry, your file could not be downloaded...\n");
   		}
		curl_easy_cleanup(c);
	}else {
		perror("Oops...Something went wrong. Please try again.\n");
	}
	fclose(download_dir);
	if(exit_succ)
		return (void*)1;
	else return (void*)-1;
}

void download_wrapper(char * dir[], char * url[], char ** times){
  printf("Received the call\n" );
  /*
    CURRENTLY NOT MULTI THREADED BECAUSE
    PTHREAD_CREATE DOES NOT WORK
    ONCE WE GET EVERYTHING TO WORK
    ADDDING THIS FUNCTIONALITY WILL
    NOT BE DIFFICULT
  */

	int idx = 0;
	pthread_t threads[1024];
	download_obj files[1024];
	while(url[idx]){
	  files[idx].dir = strdup(dir[idx]);
	  files[idx].url = strdup(url[idx]);
		pthread_create(&threads[idx],NULL,download,(void*)&files[idx]);
		idx++;
	}
 // download(&files[0]);
	idx = idx-1;
	void * ret;
	while(idx>=0){
		pthread_join(threads[idx],&ret);
		idx--;
	}

}	
void check_for_updates(char * url, long prev_mod, char * download_dir){
	CURL * curl;
 	curl_global_init(CURL_GLOBAL_DEFAULT);		//sets up the required environment variables
  curl = curl_easy_init();
  curl_easy_setopt(curl, CURLOPT_URL, url);	
  curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);			// don't get us the download file - we just need to know modification time
    curl_easy_setopt(curl, CURLOPT_FILETIME, 1L);
   	CURLcode curl_success = curl_easy_perform(curl);	
   	long time = 0;
   	if(curl_success==CURLE_OK){
   	  curl_success = curl_easy_getinfo(curl, CURLINFO_FILETIME, &time);		// retrieves the information
      if(CURLE_OK == curl_success) {
        printf("Modification time %s",ctime(&time));				/* prints out time in readable format */
      }else{
         perror("Ummm....something went wrong. Please try again.\n");
         exit(EXIT_FAILURE);
      }
      if(prev_mod<time){
      	 download_obj obj;
      	 obj.url = strdup(url);
      	 obj.dir = strdup(download_dir);

      	 download(&obj);						// the file has been updated since, so download new copy
   	  }else{
   		   fprintf(stdout, "The file is up-to-date!\n");
         //perror("Ummm....something went wrong. Please try again.\n");
   		   exit(EXIT_SUCCESS);
   	  }
   	exit(EXIT_SUCCESS);
  }
}  

 
 // int main(){
 //    char * url[2];
 //    url[0] = "http://www.marxistsfr.org/ebooks/lenin/state-and-revolution.pdf";
 //    url[1] = NULL;
 //    char * dir[2];
 //    dir[0] = "/Users/Bhawesh/GitHub/donwload_manager/state_AAAA";
 //    dir[1] = NULL;
 //    download_wrapper(dir,url,1);
 // }
