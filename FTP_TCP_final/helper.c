typedef struct{
  int code;
  char *parameter[2];
} message;

typedef struct{
  int connFd;
  char* filename;
} info;

char* create_message(int messageCode, char* para0, char* para1){
  char* buff;
  if(para1!= NULL & para0!= NULL)
    asprintf(&buff,"%d|%s|%s|",messageCode,para0,para1);
  else if(para0!=NULL)
    asprintf(&buff,"%d|%s|NULL",messageCode,para0);
  else
    asprintf(&buff,"%d|NULL|NULL",messageCode);
  return buff;
}

void separate_message(char * buff, message *message){
  char para0[strlen(buff)];
  const char s[2] = "|";
  char* token;
  token = strtok(buff,s);
  message->code = atoi(token);
  message->parameter[0]=(char*)malloc(sizeof(char)*1024);
  token = strtok(NULL,s);
  strcpy(message->parameter[0],token);
  message->parameter[1]=(char*)malloc(sizeof(char)*1024);
  token = strtok(NULL,s);
  strcpy(message->parameter[1],token);
}
