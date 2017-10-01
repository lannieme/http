#include "request_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


#define HEADER_SIZE      8192
#define BODY_SIZE        10000
#define TIME_LENGTH      31
#define FILE_TYPE_LENGTH 50

//set status code
char *STATUS_200 = "200_OK\n";
char *STATUS_204 = "204_No Content\n";
char *STATUS_404 = "404_Not Found\n";
char *STATUS_411 = "411_Length Required\n";
char *STATUS_500 = "500_Internal Server Error\n";
char *STATUS_501 = "501_Not Implemented\n";
char *STATUS_505 = "505_HTTP Version not supported\n";

static const char DATE_FORMAT_STR[]  = "%a, %d %b %Y %H:%M:%S GMT";


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

void get_file_content(FILE *fp, char *file_contents){
  int i = 0;
  char c;

  if (fp) {
      while((c = fgetc(fp)) != EOF) { 
        file_contents[i++] = c; 
      }
  }
  file_contents[i] = '\0';
}

int get_file_length(char *filename){
  struct stat st;
  stat(filename, &st);
  return st.st_size;
}

void get_current_time(char *cur_time){
  time_t t;
  t = time(NULL);

  strftime(cur_time, TIME_LENGTH, DATE_FORMAT_STR, gmtime(&t));
}

void get_last_modified(char *filename, char *file_modified){
  struct stat attrib;
  stat(filename, &attrib);

  strftime(file_modified, TIME_LENGTH, DATE_FORMAT_STR, gmtime(&(attrib.st_mtime)));
}


void handle_get(Request *request, char *response){
  char filename[1024];
  sprintf(filename, ".%s", request->http_uri);

  FILE *fp = fopen(filename, "rb");

  printf("test");


  char header[HEADER_SIZE];
  char body[BODY_SIZE];

  if (access(request->http_uri, F_OK ) == -1){
    // response for non-existing file
    strcat(header, STATUS_404);
    printf("The file does not exists \n"); 
  } else {
    // 200 OK
    strcat(header, "HTTP/1.1 ");
    strcat(header, STATUS_200);
    // Date & Time
    char date_time[TIME_LENGTH];
    get_current_time(date_time);
    strcat(header, "Date: ");
    strcat(header, date_time);
    strcat(header, "\r\n");
    // Server
    strcat(header, "Server: Liso/1.0\n");
    // Last Modified
    char last_modified_time[TIME_LENGTH];
    get_last_modified(filename, last_modified_time);
    strcat(header, "Last-Modified: ");
    strcat(header, last_modified_time);
    strcat(header, "\r\n");
    // Content-Length
    strcat(header, "Content-length: ");
    strcat(header, get_file_length(filename));
    strcat(header,"\r\n");
    // Content-Type
    char file_type[FILE_TYPE_LENGTH];
    get_file_type(filename, file_type);
    strcat(header, "Content-Type: ");
    strcpy(header, file_type);
    strcat(header,"\r\n");
    // Connection 
    // TODO
    // Add "Close"
    strcat(header, "Connection: keep-alive\r\n");
  }

  char file_content[BODY_SIZE];
  get_file_content(fp, file_content);
  fclose(fp);

  strcat(response, header);
  strcat(response, "\r\n");
  strcat(response, file_content);

}

void handle_head(Request *request, char *response){   
  char filename[1024];
  sprintf(filename, ".%s", request->http_uri);

  FILE *fp = fopen(filename, "rb");

  char header[HEADER_SIZE];
  char body[BODY_SIZE];

  if (access(request->http_uri, F_OK ) == -1){
    // response for non-existing file
    strcat(header, STATUS_404);
    printf("The file does not exists \n"); 
  } else {
    // 200 OK
    strcat(header, "HTTP/1.1 ");
    strcat(header, STATUS_200);
    // Date & Time
    char date_time[TIME_LENGTH];
    get_current_time(date_time);
    strcat(header, "Date: ");
    strcat(header, date_time);
    strcat(header, "\r\n");
    // Server
    strcat(header, "Server: Liso/1.0\n");
    // Last Modified
    char last_modified_time[TIME_LENGTH];
    get_last_modified(filename, last_modified_time);
    strcat(header, "Last-Modified: ");
    strcat(header, last_modified_time);
    strcat(header, "\r\n");
    // Content-Length
    strcat(header, "Content-length: ");
    strcat(header, get_file_length(filename));
    strcat(header,"\r\n");
    // Content-Type
    char file_type[FILE_TYPE_LENGTH];
    get_file_type(filename, file_type);
    strcat(header, "Content-Type: ");
    strcpy(header, file_type);
    strcat(header,"\r\n");
    // Connection 
    // TODO
    // Add "Close"
    strcat(header, "Connection: keep-alive\r\n");
  }
}

void handle_post(Request *request, char *response){
  if (access(request->http_uri, F_OK ) != -1 ) {
    strcat(response, STATUS_200);  
  }
  else {
    strcat(response, STATUS_204);
  }
}

//handle request and point to corresponding method
void handle_request(char *buf,int nbytes,char *response, char* agrv){
  int fd_in = open(argv, O_RDONLY);
  int index;
  char buf[8192];
  if(fd_in < 0) {
    printf("Failed to open the file\n");
    return 0;
  }
  int readRet = read(fd_in,buf,8192);
  Request *request = parse(buf,readRet,fd_in);
  // Request *request = parse(buf, nbytes,fd_in);


  if (!request) {
    strcat(response, STATUS_500);  
  }

  if (!strcmp(request->http_version , "HTTP/1.1")) {
    strcpy(response, request->http_version);
    strcat(response, " ");

    if (!strcmp(request->http_method, "GET")) {
      handle_get(request, response);
    } 
    else if (!strcmp(request->http_method, "HEAD")) {
      handle_head(request, response);        
    }
    else if (!strcmp(request->http_method, "POST")) {
      handle_post(request, response);        
    }
    else {
      strcat(response, STATUS_501);  
    }
  }
  else {
    strcpy(response, request->http_version);
    strcat(response, " ");
    strcat(response, STATUS_505);
  }
}