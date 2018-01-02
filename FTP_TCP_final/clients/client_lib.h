
#include "../helper.h"
#define PORT 5000

#define UPLOAD 21
#define DOWNLOAD 22
#define VIEW 23
#define RENAME 24
#define SHARE 25
#define VIEW_DOWNLOAD 26
#define VIEW_RENAME 27
#define DOWNLOAD_SHARE 28
#define RENAME_SHARE 29

#define UPLOAD_SV 210
#define DOWNLOAD_SV 220
#define VIEW_SV 230
#define RENAME_SV 240
#define SHARE_SV 250
#define VIEW_DOWNLOAD_SV 260
#define VIEW_RENAME_SV 270
#define DOWNLOAD_SHARE_SV 280
#define RENAME_SHARE_SV 290

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
