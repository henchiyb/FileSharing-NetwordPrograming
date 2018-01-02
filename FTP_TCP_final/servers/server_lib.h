
#include <errno.h>
#include <netdb.h>
#include <sys/wait.h>
#include "../helper.h"

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

typedef struct sockaddr sockaddr;

void signupServer(message message);
void addUsers(char* name, char* password);
void main_func();
char* getPasswordByUsername( char* name);
char* loginServer(message message);
void* sendFileToClient(char* username, char* fname);
void showErrorMessage(MYSQL* con);
void connectMysql();
char* getAllFilelOfUser();
char* getFileByShareType(int shareType);

int listenSock;
int connSock;
int reciveBytes;
int sentBytes;
char buff[2048];
char currentFile[256];
char username[256]="";
int err;
pthread_t tid;
MYSQL *con;


