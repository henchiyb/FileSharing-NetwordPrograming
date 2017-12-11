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
  close(listenSock);
}

void main_func(){
    char buff[2048];
    char cmt[30];
    while((reciveBytes = recv(connSock,buff,2048,0))){
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
    user_func();
}

void user_func(){
  while((reciveBytes = recv(connSock,buff,2048,0))){
      buff[reciveBytes]='\0';
      char* str;
      char* fname;
      message mess;
      separate_message(buff,&mess);
      if(mess.code == 23){
        printf("%s %s\n", mess.parameter[0], mess.parameter[1]);
        if(strcmp(mess.parameter[0], "fname") == 0){
          printf("Alo\n");
          struct stat st;
          fname = mess.parameter[1];
          stat(mess.parameter[1], &st);
          int size = st.st_size;
          char str[10];
          sprintf(str, "%d", size);
          char * send_file_message = create_message(23, mess.parameter[0], str);
          printf("%s\n", send_file_message);
          sentBytes= send(connSock,send_file_message,2048,0);
          // sendFileToClient(mess.parameter[0]);
        }
        if (strcmp(mess.parameter[1]  , "start") == 0 &&
          strcmp(mess.parameter[0], "download") == 0){
          // sendFileToClient(fname);
          printf("test download\n");
          printf("File:  %s\n", fname);
        }
      } else if (mess.code == 21){
        receiveFileUploadFromClient(mess.parameter[0], atoi(mess.parameter[1]));
      } else if (mess.code == 22){
        strcpy(buff,"view|true");
        send(connSock, buff, 2048, 0);
        printf("View test");
      } else if (mess.code == 24){
        rename(mess.parameter[0], mess.parameter[1]);
        strcpy(buff,"rename|true");
        send(connSock, buff, 2048, 0);
      }
      break;
  }
  user_func();
}

void addUsers(char* name, char* password){
  char buff[81];
  char* ex = (char*)malloc(81*sizeof(char));
  FILE* fptr = fopen("verifylogin.txt","a+t");
  strcpy(buff,name);
  strcpy(buff+strlen(buff),"|");
  strcpy(buff+strlen(buff),password);
  printf("%s", buff);
  while(!feof(fptr)){
    fgets(ex,81,fptr);
  }
  fprintf(fptr,"%s\n",buff);
  fclose(fptr);
  mkdir(name, 0777);
}

void addFile(int shareType, char* username, char* filename){
  char buff[2048];
  char* ex = (char*)malloc(2048*sizeof(char));
  FILE* fptr = fopen("..//file_sharing.txt","a+t");
  if (fptr == NULL){
    printf("Open error\n");
  } else {
    printf("Open\n");
  }

  char c[1];
  sprintf(c, "%d", shareType);
  strcat(buff, c);
  strcat(buff, "|");
  strcat(buff,username);
  strcat(buff,"|");
  strcat(buff,filename);

  printf("%s", buff);
  while(!feof(fptr)){
    fgets(ex,2048,fptr);
  }
  fprintf(fptr,"%s\n",buff);
  fclose(fptr);
}

void signupServer(message message){
  char* password = getPasswordByUsername(message.parameter[0]);
  char buff[30];
  if(password == NULL){
    strcpy(buff,"signup|true");
    addUsers(message.parameter[0],message.parameter[1]);
  }else {
    strcpy(buff,"signup|false");
    printf("signup false\n");
  }
  sentBytes= send(connSock,buff,1024,0);
}

char* getPasswordByUsername( char* name){
  FILE* file = fopen("verifylogin.txt","r");
  char user_name[30];
  char* password = (char*)malloc(81*sizeof(char));
  while(!feof(file)){
    fscanf(file, "%[^\|]|%[^\n]\n", user_name, password);
    if(strcmp(name,user_name)==0){
      fclose(file);
      return password;
    }
  }
  return NULL;
}

char* loginServer(message message){
  char* password = getPasswordByUsername( message.parameter[0]);
  char* buff = (char*)malloc(30*sizeof(char));
  if(password == NULL||strcmp(message.parameter[1],password)!=0)
    strcpy(buff,"login|false");
  else{
    strcpy(buff,"login|true");
    strcpy(username, message.parameter[0]);
    //sprintf(currentFolder, "client/%s/", username);
    chdir(username);
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("\tClient %s logined!\n",username);
    printf("Current working dir: %s\n", cwd);

  }
  sentBytes= send(connSock,buff,1024,0);
  return buff;
}

void* sendFileToClient(char* params){
  write(connSock, params,256);
  char* buffer = (char*)malloc(30*sizeof(char));
  FILE *fp = fopen(params,"rb");
  printf("%s\n", params);
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
  // while((bytesReceived = read(connSock, recvBuff, 1024)) > 0){
  // //   sz++;
  //   fwrite(recvBuff, 1,bytesReceived,fp);
  // }
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

