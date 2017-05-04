#include "download_manager.h"

typedef struct download_obj{
  char * url;
  char * dir;
}download_obj;

/*  Retrieves the filename from the URL.
    The filename is usually the string following the
    final '/'
    Eg. http://website.com/{some_book.pdf}
    We consider the string within brackets as the filename

*/
char * get_filename(char * url){
    url = strstr(url, "://");
    if (url) {
        url += strlen("://");
    }
    url = strrchr(url, '/');
    url++;
    return strdup(url);
}

/*
    if 'dir' exits returns 1, else 0
*/
int dir_exits(char * dir){
    DIR* dir_ = opendir(dir);
    if (ENOENT == errno){
        return 0;
    /* Directory does not exist. */
    }
    return 1;
}


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
    curl_global_init(CURL_GLOBAL_DEFAULT);    //sets up the required environment variables
    c = curl_easy_init();                     // initalize the curl object
    char * file_name = get_filename(url); 
    char dir_with_file[strlen(file_name)+strlen(dir)+1];
    strcpy(dir_with_file,dir);
    strcat(dir_with_file,file_name);
    if(!dir_exits(dir)){
        // if directory doesn't exits create with
        // read/write/execute permissions
        mkdir(dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);  
    }
    FILE * download_dir = fopen(dir_with_file,"w+");
    CURLcode success;
   // int exit_succ = 0;
   double * total_time = malloc(sizeof(double));
    if(c){
        curl_easy_setopt(c, CURLOPT_URL, url);      // uses the url to retrieve info about the file
        // set to 0L because we don't want our terminal
        // to be flooded with garbage info that we don't want
        curl_easy_setopt(c, CURLOPT_VERBOSE, 0L);    // provides meaningful message to the user
        curl_easy_setopt(c, CURLOPT_WRITEDATA, download_dir);   // write the retrieved file (from the url) to down_dir 

        // starts 'performing' the action, i.e. actually starts downloading
        // from the provied url and writes the info to the file specified
        // by download_dir
        success = curl_easy_perform(c);                         
        if(success == CURLE_OK){
            // retrieves the total number of seconds it took
            // for the file to get downloaded
            success = curl_easy_getinfo(c, CURLINFO_TOTAL_TIME, total_time);    
        }else{
            *total_time = -1;     // signifies error while downloading
        }                         // when socket sees -1, it will print the error message
        curl_easy_cleanup(c);
    }else {
        *total_time = -1;
    }
    fclose(download_dir);
    return (void*)total_time;
}

/*
*    Wraps the call to the actual download function.
*    For each download request, creates a new thread 
*    that will take care of it.
*    @param: dir  => array of download directories
*            url  => array of urls (to download from
*            time => array of times modified 
*            Each element of any array in a given index maps
*            to the elements in the other arrays at the same index
*/
double * *download_wrapper(char * *dir, char ** url, char ** times){
    
    int idx = 0;
    pthread_t threads[1024];
    download_obj files[1024];
    while(url[idx]){  
        files[idx].dir = strdup(dir[idx]);
        files[idx].url = strdup(url[idx]);
        pthread_create(&threads[idx],NULL,download,(void*)&files[idx]);
        idx++;
    }
    // contains the time taken for each file 
    // to get downloded. 
    double * * download_time = malloc(sizeof(double*)*(idx+1));   
    download_time[idx] = NULL; 
    idx = idx-1;
    void * ret = malloc(sizeof(double));
    while(idx>=0){
       pthread_join(threads[idx],&download_time[idx]);
       idx--;
    }
    return download_time;
} 


void check_for_updates(char * url, long prev_mod, char * download_dir){
    CURL * curl;
    curl_global_init(CURL_GLOBAL_DEFAULT);    //sets up the required environment variables
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url); 
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);     // don't get us the download file - we just need to know modification time
    curl_easy_setopt(curl, CURLOPT_FILETIME, 1L);
    CURLcode curl_success = curl_easy_perform(curl);  
    long time = 0;
    if(curl_success==CURLE_OK){
      curl_success = curl_easy_getinfo(curl, CURLINFO_FILETIME, &time);   // retrieves the information
      if(CURLE_OK == curl_success) {
        printf("Modification time %s",ctime(&time));        /* prints out time in readable format */
      }else{
         perror("Ummm....something went wrong. Please try again.\n");
         exit(EXIT_FAILURE);
      }
      if(prev_mod<time){
         download_obj obj;
         obj.url = strdup(url);
         obj.dir = strdup(download_dir);
         download(&obj);            // the file has been updated since, so download new copy
      }else{
         fprintf(stdout, "The file is up-to-date!\n");
         exit(EXIT_SUCCESS);
      }
    exit(EXIT_SUCCESS);
  }
}  

