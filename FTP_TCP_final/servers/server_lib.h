
#include <errno.h>
#include <netdb.h>
#include <sys/wait.h>
#include "../helper.h"

typedef struct sockaddr sockaddr;

void signupServer(message message);
void addUsers(char* name, char* password);
void main_func();
char* getPasswordByUsername( char* name);
char* loginServer(message message);
void* sendFileToClient(char* fname);

int listenSock;
int connSock;
int reciveBytes;
int sentBytes;
char buff[2048];
char currentFile[256];
char username[256]="";
int err;
pthread_t tid;


