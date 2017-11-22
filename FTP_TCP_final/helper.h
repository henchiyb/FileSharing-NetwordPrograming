#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "helper.c"

char* create_message(int messageCode, char* para0, char* para1);
void separate_message(char * buff, message *message);
