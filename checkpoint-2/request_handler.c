#include "request_handler.h"

//set status code
char *STATUS_200 = "200_OK"
char *STATUS_400 = "400_Not Found"
char *STATUS_411 = "411_Length Required"
char *STATUS_500 = "500_Internal Server Error"
char *STATUS_501 = "501_Not Implemented"
char *STATUS_505 = "505_HTTP Version not supported"


void get_file_type(char *filename, char *filetype){
	if (strstr(filename, ".html"))
    strcpy(filetype, "text/html");
  else if (strstr(filename, ".css"))
      strcpy(filetype, "text/css");
  else if (strstr(filename, ".js"))
      strcpy(filetype, "application/javascript");
  else if (strstr(filename, ".jpg"))
      strcpy(filetype, "image/jpeg");
  else if (strstr(filename, ".png"))
      strcpy(filetype, "image/png");
  else if (strstr(filename, ".gif"))
      strcpy(filetype, "image/gif");
  else
      strcpy(filetype, "text/plain");
}

static void get_file_content(){

}



void handle_get(){

}

void handle_head(){


}

void handle_post(){


}

void handle_request(){
  get_file_type(filename, filetype);
}