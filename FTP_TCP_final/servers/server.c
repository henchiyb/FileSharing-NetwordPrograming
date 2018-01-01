#include "server_lib.h"
void sig_chld(int singno){
  pid_t pid;
  int stat;
  while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0)
    printf("Child %d terminated.\n", pid);
  return;
}

void main(){
  char buff[2048];
  pid_t pid;
  struct sockaddr_in serverAddr, clientAddr;
  int clientAddrLength;
  if((listenSock = socket(AF_INET,SOCK_STREAM,0)) == -1){
    printf("ERROR: error in socket()\n"); return;
  }
  // khoi tao cac dia chi
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(5000);
  serverAddr.sin_addr.s_addr=htonl(INADDR_ANY);
  if(bind(listenSock,(struct sockaddr *)&serverAddr, sizeof(sockaddr))==-1){
    printf("ERROR: error in bind()\n");
  return;
  }
  listen(listenSock,10);
  clientAddrLength = sizeof(clientAddr);
  connectMysql();
  while(1){
    connSock = accept(listenSock,(sockaddr *)&clientAddr,(socklen_t*)&clientAddrLength);
    if(connSock<0) {printf("ERROR: error in accept()\n");exit(0);}
    printf("client connect\n");
    if((pid=fork()) == 0){
      close(listenSock);
      main_func();
      close(connSock);
      exit(0);
    }
    signal(SIGCHLD,sig_chld);
    close(connSock);
  }
  mysql_close(con);
  close(listenSock);
}

void main_func(){
    char buff[2048];
    char cmt[30];
    while((reciveBytes = recv(connSock,buff,2048,0))){
      if (buff != NULL){
        buff[reciveBytes]='\0';
        message mess;
        separate_message(buff,&mess);
        if(mess.code == 2){
          strcpy(cmt,loginServer(mess));
        }
        else if(mess.code == 1){
          signupServer(mess);
        }
        if(strcmp(cmt,"login|true")==0){
        strcpy(username,mess.parameter[0]);
        printf("Client %s logined!\n",username);
        break;
        }
      }
    }
    user_func();
}

void user_func(){
  char* fname;
  while((reciveBytes = recv(connSock,buff,2048,0))){
    if (buff != NULL){
      buff[reciveBytes]='\0';
      char* str;
      message mess;
      separate_message(buff,&mess);
      if(mess.code == 23){
        if(strcmp(mess.parameter[0], "fname") == 0){
          struct stat st;
          fname = mess.parameter[1];
          char buffer[2048];
          if (!file_exist(fname)){
            strcpy(buffer, "23|download|not_exist|");
            send(connSock, buffer, 2048, 0);
            printf("%s\n", buffer);
          } else {
            stat(fname, &st);
            int size = st.st_size;
            printf("Size: %d\n", size);
            char str[10];
            sprintf(str, "%d", size);
            char * send_file_message = create_message(23, mess.parameter[0], str);
            printf("%s\n", send_file_message);
            sentBytes= send(connSock,send_file_message,2048,0);
            recv(connSock, buffer, 2048, 0);
            if (strcmp(buffer, "23|download|ready|") == 0){
              char buffer_start[2048];
              strcpy(buffer_start, "23|download|start|");
              sentBytes= send(connSock,buffer,2048,0);
              printf("Test down: %s\n", fname);
              sendFileToClient(username, fname);
              printf("test download\n");
            }
          }
        }
      } else if (mess.code == 21){
        receiveFileUploadFromClient(mess.parameter[0], atoi(mess.parameter[1]));
      } else if (mess.code == 22){
        char* listFile = getAllFilelOfUser();
        printf("%s\n", listFile);
        char* view_all_message = create_message(22, "view", listFile);
        send(connSock, view_all_message, strlen(view_all_message), 0);
        printf("View test: %s", view_all_message);
      } else if (mess.code == 24){
          if (file_exist(mess.parameter[0]) && !file_exist(mess.parameter[1])){
            updateFilename(mess.parameter[0], mess.parameter[1]);
            strcpy(buff,"rename|true");
            send(connSock, buff, 2048, 0);
          } else {
            strcpy(buff,"rename|false");
            send(connSock, buff, 2048, 0);
          }
      } else if (mess.code == 25){
        updateShareType(mess.parameter[0], atoi(mess.parameter[1]));
        strcpy(buff,"share|true");
        send(connSock, buff, 2048, 0);
      } else if (mess.code == 26){
        downloadFileShare(mess.parameter[0], mess.parameter[1]);
        printf("Share down: %s - %s\n", mess.parameter[0], mess.parameter[1]);
      } else if (mess.code == 27){
        char* listFile = getFileByShareType(2);
        printf("%s\n", listFile);
        char* view_rename = create_message(27, "view", listFile);
        send(connSock, view_rename, strlen(view_rename), 0);
        printf("View test: %s", view_rename);
      } else if (mess.code == 28){
        char* listFile = getFileByShareType(3);
        printf("%s\n", listFile);
        char* view_download = create_message(28, "view", listFile);
        send(connSock, view_download, strlen(view_download), 0);
        printf("View test: %s", view_download);
      } else if (mess.code == 29){
        char *token;
        char *token1;
        char *search = " ";
        token = strtok(mess.parameter[1], search);
        // Token will point to "WORDS".
        token1 = strtok(NULL, search);
        printf("token %s %s\n", token, token1);
        if (file_exist(token) && !file_exist(token1)){
          updateFilenameShare(mess.parameter[0], token, token1);
          strcpy(buff,"rename|true");
          send(connSock, buff, 2048, 0);
        } else {
          strcpy(buff,"rename|false");
          send(connSock, buff, 2048, 0);
        }

      }
      break;
    }
  }
  user_func();
}

void addUsers(char* name, char* password){
  char* query;
  asprintf(&query,"INSERT INTO users(username, password) VALUES('%s','%s')",
    name, password);
  mysql_query(con, query);
  mkdir(name, 0777);
}

void addFile(int shareType, char* username, char* filename){
  char* query;
  asprintf(&query,"SELECT * FROM users WHERE username = '%s'", username);
  mysql_query(con, query);
  MYSQL_RES *result = mysql_store_result(con);

  MYSQL_ROW row;
  row = mysql_fetch_row(result);
  asprintf(&query, "INSERT INTO files(filename, share_type, user_id) VALUES('%s',%d ,%d)",
    filename, shareType, atoi(row[0]));
  mysql_query(con, query);
}

void signupServer(message message){
  char* query;
  asprintf(&query,"SELECT * FROM users WHERE username = '%s' AND password = '%s'",
    message.parameter[0], message.parameter[1]);
  mysql_query(con, query);
  MYSQL_RES *result = mysql_store_result(con);
  int num_fields = mysql_num_fields(result);

  MYSQL_ROW row;
  MYSQL_FIELD *field;

  if ((row = mysql_fetch_row(result)) == NULL)
  {
    strcpy(buff,"signup|true");
    addUsers(message.parameter[0],message.parameter[1]);
  } else{
    strcpy(buff,"signup|false");
    printf("signup false\n");
  }
  sentBytes= send(connSock,buff,1024,0);
  mysql_free_result(result);
}

char* loginServer(message message){
  char* query;
  asprintf(&query,"SELECT * FROM users WHERE username = '%s' AND password = '%s'",
    message.parameter[0], message.parameter[1]);
  mysql_query(con, query);
  MYSQL_RES *result = mysql_store_result(con);
  int num_fields = mysql_num_fields(result);

  MYSQL_ROW row;
  MYSQL_FIELD *field;

  if ((row = mysql_fetch_row(result)) == NULL)
  {
      strcpy(buff,"login|false");
  } else{
    strcpy(buff,"login|true");
    strcpy(username, message.parameter[0]);
    chdir(username);
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("\tClient %s logined!\n",username);
    printf("Current working dir: %s\n", cwd);

  }
  sentBytes= send(connSock,buff,1024,0);
  mysql_free_result(result);
  return buff;
}

void* sendFileToClient(char* user_name, char* params){
  write(connSock, params,256);
  char* buffer = (char*)malloc(30*sizeof(char));
  char* path;
  asprintf(&path, "..//%s/%s",user_name, params);
  FILE *fp = fopen(path,"rb");
  printf("%s\n", path);
  if(fp == NULL){
    printf("File open error\n");
    // strcpy(buffer,"openfile|false");
    // sentBytes= send(connSock,buffer,1024,0);
    return 0;
  } else {
    printf("OPEN\n");
  }
    while(1){
      /* First read file in chunks of 256 bytes */
      unsigned char buff[1024]={0};
      int nread = fread(buff, 1, 1024, fp);
      if(nread > 0){
        write(connSock, buff, nread);
      }
      if (nread < 1024){
        if (feof(fp)){
          printf("End of file\n");
          printf("File transfer completed for id: %d\n",connSock);
        }
        if (ferror(fp))
          printf("Error reading\n");
        break;
      }
    }
}

void receiveFileUploadFromClient(char* params, int size){
  char buffer[2048];
  if (file_exist(params)){
    strcpy(buffer, "upload|exist");
    send(connSock, buffer, 2048, 0);
    printf("%s\n", buffer);
  } else {
    strcpy(buffer, "upload|start");
    send(connSock, buffer, 2048, 0);
    printf("%s\n", buffer);
    int bytesReceived = 0;
    char recvBuff[1024];
    memset(recvBuff, '0', sizeof(recvBuff));
    read(connSock, params, 256);
    printf("File Name: %s\n",params);
    printf("Receiving file...");

    FILE *fp;
    fp = fopen(params, "ab");
    if(NULL == fp){
      printf("Error opening file");
      return 1;
    } else {
          printf("Opened file");
    }
    long int sz = 1;
    printf("%d\n", size);
    while(sz < size){
      bytesReceived = read(connSock, recvBuff, 1024);
      sz+=1024;
      printf("%d\n", sz);
      fwrite(recvBuff, 1,bytesReceived,fp);
    }
    printf("%d\n", bytesReceived);

    if(bytesReceived < 0){
      printf("\n Send Error \n");
    }
    addFile(1, username, params);
    printf("\nFile OK....Completed\n");
  }
}

void updateShareType(char* filename, int shareType){
  char* query;
  asprintf(&query,"SELECT * FROM users WHERE username = '%s'", username);
  mysql_query(con, query);
  MYSQL_RES *result = mysql_store_result(con);
  MYSQL_ROW row;
  row = mysql_fetch_row(result);
  asprintf(&query, "UPDATE files SET share_type = %d WHERE filename = '%s' AND user_id = %d",
    shareType, filename, atoi(row[0]));
  mysql_query(con, query);
  mysql_free_result(result);
}

void updateFilename(char* oldname, char* newname){
  char* query;
  asprintf(&query, "UPDATE files SET filename = '%s' WHERE filename = '%s'",
    newname, oldname);
  mysql_query(con, query);
  rename(oldname, newname);
}

void updateFilenameShare(char* user_name, char* oldname, char* newname){
  char* query;
  char* old_name_path;
  char* new_name_path;
  asprintf(&old_name_path,"..//%s//%s", user_name, oldname);
  asprintf(&new_name_path,"..//%s//%s", user_name, newname);

  asprintf(&query, "UPDATE files SET filename = '%s' WHERE filename = '%s'",
    newname, oldname);
  mysql_query(con, query);
  rename(old_name_path, new_name_path);
}

char* getAllFilelOfUser(){
  char* listFile = " ";
  char* query;
  asprintf(&query, "SELECT u.username, f.filename, f.share_type FROM users u JOIN files f WHERE u.username = '%s' AND u.user_id = f.user_id",
    username);
  mysql_query(con, query);
  MYSQL_RES *result = mysql_store_result(con);
  MYSQL_ROW row;
  printf("Chay\n");
  while((row = mysql_fetch_row(result))){
    char* str;
    asprintf(&str, " User: %s --- Filename: %s --- Share_type: %s \n",
    row[0], row[1], row[2]);
    asprintf(&listFile, "%s %s", listFile, str);
  }
  mysql_free_result(result);
  return listFile;
}

char* getFileByShareType(int shareType){
  char* listFile = " ";
  char* query;
  asprintf(&query, "SELECT u.username, f.filename, f.share_type FROM users u JOIN files f WHERE f.share_type = %d AND u.user_id = f.user_id",
    shareType);
  mysql_query(con, query);
  MYSQL_RES *result = mysql_store_result(con);
  MYSQL_ROW row;
  while((row = mysql_fetch_row(result))){
    char* str;
    asprintf(&str, " User: %s --- Filename: %s --- Share_type: %s \n",
    row[0], row[1], row[2]);
    asprintf(&listFile, "%s %s", listFile, str);
  }
  mysql_free_result(result);
  return listFile;
}

void downloadFileShare(char* user_name, char* file_name){
  char* query;
  asprintf(&query, "SELECT u.username, f.filename FROM users u JOIN files f WHERE u.username = '%s' AND f.filename = '%s' AND f.share_type = 3 AND u.user_id = f.user_id",
    user_name, file_name);
  mysql_query(con, query);
  MYSQL_RES *result = mysql_store_result(con);
  MYSQL_ROW row;
  char buffer[2048];
  if((row = mysql_fetch_row(result)) != NULL){
      struct stat st;
      char* path;
      asprintf(&path, "..//%s/%s", row[0], row[1]);
      stat(path, &st);
      int size = st.st_size;
      char str[10];
      sprintf(str, "%d", size);
      char * send_file_message = create_message(26, "fname", str);
      sentBytes= send(connSock,send_file_message,2048,0);
      recv(connSock, buffer, 2048, 0);
      if (strcmp(buffer, "26|download|ready|") == 0){
        char buffer_start[2048];
        strcpy(buffer_start, "26|download|start|");
        sentBytes= send(connSock,buffer,2048,0);
        sendFileToClient(row[0], row[1]);
        printf("test download\n");
      }
  } else {
    strcpy(buffer, "26|download|not_exist|");
    send(connSock, buffer, 2048, 0);
    printf("%s\n", buffer);
  }
}

void showErrorMessage(MYSQL* con){
   fprintf(stderr, "%s\n", mysql_error(con));
   exit(1);
}

void connectMysql(){
  con = mysql_init(NULL);

  if (con == NULL)
    showErrorMessage(con);

  if (mysql_real_connect(con, "localhost", "root", "ngoan123",
          "file_share", 0, NULL, 0) == NULL)
    showErrorMessage(con);
}

int file_exist (char *filename)
{
  struct stat   buffer;
  return (stat (filename, &buffer) == 0);
}

