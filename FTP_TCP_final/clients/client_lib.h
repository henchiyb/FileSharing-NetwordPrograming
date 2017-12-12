
#include "../helper.h"

struct sockaddr_in serverAddr;
char* createUserMessage(int verify);
void clientHasFunc();
void signupClient();
int menuVerifyUser();
void verifyUser();
void receiveFileFromServer();
void* uploadFileToServer(char* params);
char* create_new_filename(char* filename, int i);

int reciveBytes;
int sentBytes;
int sockfd;
char buff[2048];
char username[2048];
char fname[30];
int err;
pthread_t tid;
