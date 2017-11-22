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
        err = pthread_create(&tid, NULL, SendFileToClient(fname), &connSock);
      }
      else if(mess.code == 1){
        signupServer(mess);
      }
      else if(mess.code == 21){
        printf("%s\n", username);
        asprintf(&str, "%s/%s", username, "Mars4_5.jar");
        err = pthread_create(&tid, NULL, SendFileToClient(str), &connSock);
        if (err != 0)
          printf("\ncan't create thread :[%s]", strerror(err));
      }
      if(strcmp(cmt,"login|true")==0){
      strcpy(username,mess.parameter[0]);
      printf("Client %s logined!\n",username);
      // break;
      }
    }
    //user_func();
}

void user_func(){
  while((reciveBytes = recv(connSock,buff,2048,0))){
      buff[reciveBytes]='\0';
      char* str;
      message mess;
      separate_message(buff,&mess);
      if(mess.code == 21){

      }
      break;
  }
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

void signupServer(message message){
  char* password = findUserName(message.parameter[0]);
  char buff[30],*system_mess;
  if(password == NULL){
    strcpy(buff,"signup|true");
    addUsers(message.parameter[0],message.parameter[1]);
  }else {
    strcpy(buff,"signup|false");
    printf("signup false\n");
  }
  sentBytes= send(connSock,buff,1024,0);
  printf("test send\n");
}

char* findUserName( char* name){
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
  char* password = findUserName( message.parameter[0]);
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

void* SendFileToClient(char* fname){
  write(connSock, fname,256);
  char* path;
  //asprintf(&path,"%s/%s",username,fname);

  printf("%s\n", path);
  printf("%s\n", fname);
  FILE *fp = fopen(fname,"rb");
    if(fp==NULL){
      printf("File opern error");
      return 1;
    }
/* Read data from file and send it */
    while(1){
      /* First read file in chunks of 256 bytes */
      unsigned char buff[1024]={0};
      int nread = fread(buff,1,1024,fp);
      //printf("Bytes read %d \n", nread);
      /* If read was success, send data. */
      if(nread > 0){
        //printf("Sending \n");
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
