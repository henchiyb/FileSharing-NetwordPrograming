#include "client_lib.h"

void main(int argc, char* argv[1]){
  char temp[256];
  sockfd = socket(AF_INET,SOCK_STREAM,0);
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(PORT);
  strcpy(temp,argv[1]);
  if((inet_aton(temp,&serverAddr.sin_addr)==0)){
    printf("ERROR: error in inet_aton()\n");
    return;
  }
  if(connect(sockfd,(struct sockaddr*)&serverAddr,sizeof serverAddr)!=0){
    printf("ERROR: error in connect()\n");
    return;
  }
  clientHasFunc();
  close(sockfd);
}

void clientHasFunc(){
  verifyUser();
  fileControl();
}

void verifyUser(){
  int is_login=0;
  char* buff = (char*)malloc(sizeof(char));
  do{
    int choice;
    choice = menuVerifyUser();
    char c;
    switch(choice){
      case 1:
      signupClient();
      break;
      case 2:
      is_login = loginClient();
      break;
      case 3:
      exit(0);
      default:
      printf("\nWrong input!");
    }
    if(is_login==1) break;
  }while(1);
}

void fileControl(){
    // char* buff = (char*)malloc(sizeof(char));
    int choice;
    choice = menuFileControl();
    char c;
    switch(choice){
      case 1:
        printf("\nFilename to upload: ");
        gets(fname);
        FILE *fp = fopen(fname,"rb");
        if(fp==NULL){
          printf("File not found!\n");
        } else {
          printf("File open\n");
          fclose(fp);
          struct stat st;
          stat(fname, &st);
          int size = st.st_size;
          char str[10];
          sprintf(str, "%d", size);
          char * upload_message = create_message(UPLOAD,fname,str);
          send(sockfd,upload_message,strlen(upload_message),0);
          char * buffer[2048];
          recv(sockfd, buffer, 2048, 0);
          if (strcmp(buffer, "210|upload|exist") == 0)
            printf("File exist! please rename\n");
          else if (strcmp(buffer, "210|upload|start") == 0)
            uploadFileToServer(fname);
        }
        break;
      case 2:
        printf("\nFilename to download: ");
        gets(fname);
        char * buffer[2048];
        char * download_message = create_message(DOWNLOAD,"fname",fname);
        send(sockfd,download_message,strlen(download_message),0);
        recv(sockfd, buffer ,2048, 0);
        message mess;
        separate_message(buffer, &mess);
        if (strcmp(mess.parameter[0], "fname") == 0){
          int size = atoi(mess.parameter[1]);
          char * download_start = create_message(DOWNLOAD,"download","ready");
          send(sockfd,download_start,strlen(download_start),0);
          recv(sockfd, buffer ,2048, 0);
          printf("%s\n", buffer);
          if (strcmp(buffer, "220|download|start|") == 0){
            printf("Download start\n");
            receiveFileFromServer(fname, size);
          }
        } else if (strcmp(mess.parameter[1], "not_exist") == 0){
          printf("File download not exist\n");
        }
        break;
      case 3:
        printf("View\n");
        char * view_ready = create_message(VIEW,"view","ready");
        send(sockfd,view_ready,strlen(view_ready),0);
        recv(sockfd, buff, 2048, 0);
        message mess_view;
        separate_message(buff, &mess_view);
        if (mess_view.code == VIEW_SV &&
            strcmp(mess_view.parameter[0], "view") == 0){
          printf("All file of user: \n");
          printf("%s\n", mess_view.parameter[1]);
        } else {
          printf("View false\n");
        }
        break;
      case 4:
        printf("\nFilename to rename: ");
        gets(fname);
        char newname[2048];
        printf("\nNew name: ");
        gets(newname);
        char * rename_message = create_message(RENAME, fname, newname);
        send(sockfd,rename_message,strlen(rename_message),0);
        recv(sockfd,buff,2048,0);
        if (strcmp(buff, "240|rename|true") == 0){
          printf("Rename success!\n");
        } else {
          printf("Rename failed!\n");
        }
        break;
      case 5:
        printf("\nFilename to change share: ");
        gets(fname);
        int shareType = menuSelectShareType();
        char str[1];
        sprintf(str, "%d", shareType);
        char * change_share_message = create_message(SHARE, fname, str);
        send(sockfd,change_share_message,strlen(change_share_message),0);
        recv(sockfd,buff,2048,0);
        if (strcmp(buff, "250|share|true") == 0){
          printf("Share success!\n");
        } else {
          printf("Share failed!\n");
        }
        break;
      case 6:
        printf("View rename\n");
        char * view_rename_ready = create_message(VIEW_RENAME,"view","ready");
        send(sockfd,view_rename_ready,strlen(view_rename_ready),0);
        recv(sockfd, buff, 2048, 0);
        message mess_view_rename;
        separate_message(buff, &mess_view_rename);
        if (mess_view_rename.code == VIEW_RENAME_SV &&
            strcmp(mess_view_rename.parameter[0], "view") == 0){
          printf("All file can rename in server: \n");
          printf("%s\n", mess_view_rename.parameter[1]);
        } else {
          printf("View false\n");
        }
        break;
      case 7:
        printf("View download\n");
        char * view_download_ready = create_message(VIEW_DOWNLOAD,"view","ready");
        send(sockfd,view_download_ready,strlen(view_download_ready),0);
        recv(sockfd, buff, 2048, 0);
        message mess_view_download;
        separate_message(buff, &mess_view_download);
        if (mess_view_download.code == VIEW_DOWNLOAD_SV &&
            strcmp(mess_view_download.parameter[0], "view") == 0){
          printf("All file can download in server: \n");
          printf("%s\n", mess_view_download.parameter[1]);
        } else {
          printf("View false\n");
        }
        break;
      case 8:
        printf("\nUser has file: ");
        char user_name[2048];
        gets(user_name);
        printf("\nFilename to rename: ");
        gets(fname);
        char newnames[2048];
        printf("\nNew name: ");
        gets(newnames);
        strcat(fname, " ");
        strcat(fname, newnames);
        char * rename_message_share = create_message(RENAME_SHARE, user_name, fname);
        send(sockfd,rename_message_share,strlen(rename_message_share),0);
        printf("%s\n", rename_message_share);
        recv(sockfd,buff,2048,0);
        if (strcmp(buff, "290|rename|true") == 0){
            printf("Rename success!\n");
        } else if (strcmp(buff, "290|rename|false") == 0){
          printf("Rename failed!\n");
        }
        break;
      case 9:
        printf("\nFilename to download: ");
        gets(fname);
        char user_has_file[2048];
        printf("\nUser has file: ");
        gets(user_has_file);
        char * download_share_message = create_message(DOWNLOAD_SHARE,user_has_file,fname);
        send(sockfd, download_share_message, strlen(download_share_message),0);
        char buff_share[2048];
        recv(sockfd, buff_share ,2048, 0);
        message mess_share;
        separate_message(buff_share, &mess_share);
        if (strcmp(mess_share.parameter[0], "fname") == 0){
          int size = atoi(mess_share.parameter[1]);
          char * download_start = create_message(DOWNLOAD_SHARE,"download","ready");
          send(sockfd,download_start,strlen(download_start),0);
          recv(sockfd, buffer ,2048, 0);
          if (strcmp(buffer, "280|download|start|")){
            printf("Download start\n");
            receiveFileFromServer(fname, size);
          }
        } else if (strcmp(buffer, "280|download|not_exist|") == 0){
          printf("File download not exist\n");
        }
        break;
      case 10:
        exit(0);
      default:
        printf("\nWrong input!");
        break;
    }
  fileControl();
}

int menuVerifyUser(){
  int choice;
  printf("\n\t*************************\n");
  printf("\t*      VERIFY USER      *\n");
  printf("\t*************************\n");
  printf("\t* 1. Sign up            *\n");
  printf("\t* 2. Login              *\n");
  printf("\t* 3. Exit               *\n");
  printf("\t*************************\n");
  printf("\t---> choice: ");
  scanf("%d%*c", &choice);
  while(choice!=1 && choice!=2 && choice!=3){
    printf("Your choice is invalid. Please choice from 1 to 3.\n");
    printf("\t---> choice: ");
    scanf("%d%*c",&choice);
  }
  return choice;
}

int menuSelectShareType(){
  int choice;
  printf("\n\t*************************\n");
  printf("\t*      Share Type       *\n");
  printf("\t*************************\n");
  printf("\t* 1. Private            *\n");
  printf("\t* 2. Rename             *\n");
  printf("\t* 3. Download           *\n");
  printf("\t*************************\n");
  printf("\t---> choice: ");
  scanf("%d%*c", &choice);
  while(choice!=1 && choice!=2 && choice!=3){
    printf("Your choice is invalid. Please choice from 1 to 3.\n");
    printf("\t---> choice: ");
    scanf("%d%*c",&choice);
  }
  return choice;
}

int menuFileControl(){
  int choice;
  printf("\n\t**********************************\n");
  printf("\t*           FILE CONTROL         *\n");
  printf("\t**********************************\n");
  printf("\t* 1. Upload                      *\n");
  printf("\t* 2. Download                    *\n");
  printf("\t* 3. View                        *\n");
  printf("\t* 4. Rename                      *\n");
  printf("\t* 5. Share                       *\n");
  printf("\t* 6. View Renameable File        *\n");
  printf("\t* 7. View Downloadable File      *\n");
  printf("\t* 8. Rename Shared File          *\n");
  printf("\t* 9. Download Shared File        *\n");
  printf("\t* 10. Exit                       *\n");
  printf("\t**********************************\n");
  printf("\t---> choice: ");
  scanf("%d%*c", &choice);
  while(choice!=1 && choice!=2 && choice!=3 && choice!=4 && choice!=5
    && choice!=6 && choice!=7 && choice!=8 && choice!=9 && choice!=10){
    printf("Your choice is invalid. Please choice from 1 to 10.\n");
    printf("\t---> choice: ");
    scanf("%d%*c",&choice);
  }
  return choice;
}

char* createUserMessage(int verify){
  char name[30];
  char pass[30];
  char c;
  printf("UserName: ");
  gets(name);
  printf("Password: ");
  gets(pass);
  if(verify == 2)
    return create_message( 2, name, pass);
  else if (verify == 1)
    return create_message( 1, name, pass);
}

void signupClient(){
  char buff[2048];
  printf("\n\t*************************\n");
  printf("\t*        SIGN UP        *\n");
  printf("\t*************************\n");
      char* signup_result = createUserMessage(1);
      printf("%s\n", signup_result);
      send (sockfd,signup_result,strlen(signup_result),0);
      recv(sockfd,buff,2048,0);
      if(strcmp(buff,"signup|false")==0)
         printf("Account already exist!\n\n");
      else {
         printf("Sign up sucessed!\n\n");
      }
}

int loginClient(){
  char buff[2048];
  int is_login=0;
  printf("\n\t************************\n");
  printf("\t*         LOGIN        *\n");
  printf("\t************************\n");
      char* login_result;
      login_result = createUserMessage(2);
      send (sockfd,login_result,2048,0);
      message mess;
      separate_message(login_result,&mess);
      recv(sockfd,buff,2048,0);
      printf("%s\n", buff);
      if(strcmp(buff,"login|true")==0){
        printf("Login sucessed!\n\n");
        is_login = 1;
        strcpy(username,mess.parameter[0]);
      }
      else
        printf("Account or password is wrong!\n\n");
      return is_login;
}

void receiveFileFromServer(char* params, int size){
  int bytesReceived = 0;
  char recvBuff[1024];
  memset(recvBuff, '0', sizeof(recvBuff));
  read(sockfd, params, 256);
  printf("File Name: %s\n",params);
  printf("File size: %d\n",size);

  char newname[2048];
  strcpy(newname, params);
  int i = 0;
  while(file_exist(newname)){
    i++;
    char* tmp;
    asprintf(&tmp, "%s (%d)", params, i);
    strcpy(newname, tmp);
  }

/* Create file where data will be stored */
  FILE *fp;
  fp = fopen(newname, "ab");
  if(NULL == fp){
    printf("Error opening file\n");
    return 0;
  } else {
    printf("Opened\n");
  }
  printf("Receiving file...\n");
  long int sz=1;
   // Receive data in chunks of 256 bytes
  while(sz < size){
    bytesReceived = read(sockfd, recvBuff, 1024);
    sz+=1024;
    printf("%d\n", sz);
    fwrite(recvBuff, 1,bytesReceived,fp);
  }
  if(bytesReceived < 0){
    printf("\n Send Error \n");
  }
  printf("\nFile OK....Completed\n");
}

void* uploadFileToServer(char* params){
  write(sockfd, params,256);
  char* path;
  asprintf(&path,"%s/%s", username, params);
  FILE *fp = fopen(params,"rb");
  if(fp==NULL){
    printf("File not found!\n");
    return 1;
  } else {
    printf("File open\n");
  }
  int size;
    while(1){
      /* First read file in chunks of 256 bytes */
      unsigned char buff[1024]={0};
      int nread = fread(buff,1,1024,fp);
      if(nread > 0){
        write(sockfd, buff, nread);
      }
      if (nread < 1024){
        if (feof(fp)){
          printf("End of file\n");
          printf("File transfer completed for id: %d\n",sockfd);
        }
        if (ferror(fp))
          printf("Error receiving\n");
        break;
      }
    }
}

int file_exist (char *filename)
{
  struct stat   buffer;
  return (stat (filename, &buffer) == 0);
}



