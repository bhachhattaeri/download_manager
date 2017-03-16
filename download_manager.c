

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
