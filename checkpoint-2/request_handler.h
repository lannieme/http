#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include "parse.h"

#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

void handle_request(char *buf,int nbytes,char *response);
void handle_get(Request *request, char *response);
void handle_head(Request *request, char *response);
void handle_post(Request *request, char *response);

#endif 