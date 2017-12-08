#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<stdio.h>

#define ENV_VAR_LENGTH 256
/*char **environ_main;
int environ_length;
char*PS1_SBUSH;

char *token1(char *command, int x,int y){
  //printf("In Tokeninzing %d %d \n",x,y);
  char *argtemp = (char *)malloc(sizeof(char) * (y-x+2));//mmap(NULL,sizeof(char) * (y-x+2),PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_PRIVATE,-1,0);
  if(!argtemp){
    //dedeprintf("Error: Memory Allocation failure in the sbu shell for the token buffer\n");
    exit(1);
  }
  int i=0;
  while(x<=y){
    //printf("Inside the while loop of tokenizing");
    argtemp[i++] = command[x++];
  }
  argtemp[i] = '\0';
  //printf("Argtemp in token function is: %s\n",argtemp);
  return argtemp;
}

char *getenv(char *name){
  int env_i=0;
  char *s= malloc(sizeof(char) * ENV_VAR_LENGTH);
  while(*(environ_main+env_i) != '\0'){
      int arg_i=0,end=0;
      //puts(environ_main[env_i]);
      while(environ_main[env_i][arg_i] != '='){
        end=arg_i;
        arg_i++;
      }
      arg_i++;
      if(strcmp(token1(environ_main[env_i],0,end),name) == 0){
        int start = arg_i;
        while(environ_main[env_i][arg_i] != '\0'){
          end=arg_i;
          arg_i++;
        }
        s=token1(environ_main[env_i],start,end);
      }
      //else
        //free(s);
      env_i++;
  }
  //puts("Inside getenv\n");
  //puts(s);
  //if(*(s) == '\0')
    //puts("Not found");
  return s;
}
int setenv(char *name,char *value, int overwrite)
{  
  int found =0;
  int env_i=0;
  environ_length=0;
  char **new_environ;
  //puts(name);
  //puts(value);
  while(*(environ_main+env_i) != '\0'){
      environ_length++;
      env_i++;
      //puts(environ_main[env_i]);
  }
  char *gar = getenv(name);
  if(*gar){
    found =1;
    //puts("found\n");
  }
  if(found){
    new_environ = malloc(sizeof(char *) * (environ_length+3));
    env_i=0;
    while(*(environ_main+env_i) != '\0'){
      int arg_i=0,end=0;
      //char *s = malloc(sizeof(char) * ENV_VAR_LENGTH);
      while(environ_main[env_i][arg_i] != '='){
        end=arg_i;
        arg_i++;
      }
      arg_i++;
      if(strcmp(token1(environ_main[env_i],0,end),name) == 0){
        new_environ[env_i] = malloc(sizeof(char) * ENV_VAR_LENGTH);
        strcpy(new_environ[env_i],name);
        char *delimiter="=";
        strcat(new_environ[env_i],delimiter);
        strcat(new_environ[env_i],value);
      }
      else{
      new_environ[env_i] = malloc(sizeof(char) * ENV_VAR_LENGTH);
      strcpy(new_environ[env_i],environ_main[env_i]);
      }
      env_i++;
    }
  }
  else{
    new_environ = malloc(sizeof(char *) * (environ_length+2));
    env_i=0;
    while(*(environ_main+env_i) != '\0'){
      new_environ[env_i] = malloc(sizeof(char) * ENV_VAR_LENGTH);
      strcpy(new_environ[env_i],environ_main[env_i]);
      //puts(new_environ[env_i]);
      //puts(new_environ[env_i]);
      env_i++;
    }
     new_environ[env_i] = malloc(sizeof(char) * ENV_VAR_LENGTH);
     strcat(new_environ[env_i],name);
     char *delimiter = "=";
     strcat(new_environ[env_i],delimiter);
     strcat(new_environ[env_i],value);
     env_i++;
  }
    new_environ[env_i] = malloc(sizeof(char) * ENV_VAR_LENGTH);
    *(new_environ+env_i) = '\0';
    environ_main = new_environ;
  return 0;
}

char *export(char **com_args){
  return NULL;
}*/
/*char *export(char **com_args){
      int start=0,end=0,i=0;
      char **exp_args = (char **)malloc(sizeof(char *) * 4);//mmap(NULL,sizeof(char *) * 4,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_PRIVATE,-1,0);
      exp_args[0] = "setenv";
      while(com_args[1][i] != '='){
        if(com_args[1][i] == '\0'){
          //printf("Error while setting environment varibale, expecting a var and a value\n");
          return NULL;
        }
        end=i;
        i++; 
      }
      exp_args[1] = token1(com_args[1],start,end);
      start = i+1;
      i++;
      short int quotes_used = 0;
      if(com_args[1][i] == '"'){
        start++;
        quotes_used = 1;
      }
      while(com_args[1][i] != '\0'){
        end=i;
        i++;
      }
      if(com_args[1][i-1] == '"')
        if(quotes_used == 1)
          end--;
      if(start<=end)
        exp_args[2] = token1(com_args[1],start,end);
      else{
        //printf("Error while setting environment varibale, expecting a value\n");
        return NULL;
      }
      *(exp_args+3) = '\0';
      if(strcmp(exp_args[1],"PS1") == 0){
        strcpy(PS1_SBUSH,exp_args[2]);
        exp_args[1] = "PS1_SBUSH";
      }
      if(setenv(exp_args[1],exp_args[2],1) == 0){
        //char *s=getenv(exp_args[1]);
        //printf("**%s**",s);
        return NULL;
      }  
      else{
        return NULL;
      }  
}
*/

/*
int setenv(char *name,char *value, int overwrite)
{  
  int found =0;
  int env_i=0;
  environ_length=0;
  char **new_environ;
  puts(name);
  puts(value);
  while(*(environ_main+env_i) != '\0'){
      environ_length++;
      env_i++;
      puts(environ_main[env_i]);
  }
  char *gar = getenv(name);
  if(*gar){
    found =1;
    puts("found\n");
  }
  if(found){
    new_environ = malloc(sizeof(char *) * (environ_length+2));
    env_i=0;
    while(*(environ_main+env_i) != '\0'){
      int arg_i=0,end=0;
      //char *s = malloc(sizeof(char) * ENV_VAR_LENGTH);
      while(environ_main[env_i][arg_i] != '='){
        end=arg_i;
        arg_i++;
      }
      arg_i++;
      if(strcmp(token1(environ_main[env_i],0,end),name) == 0){
        new_environ[env_i] = malloc(sizeof(char) * ENV_VAR_LENGTH);
        strcpy(environ_main[env_i],name);
        char *delimiter="=";
        strcat(new_environ[env_i],delimiter);
        strcat(new_environ[env_i],value);
      }
      else{
      new_environ[env_i] = malloc(sizeof(char) * ENV_VAR_LENGTH);
      new_environ[env_i] = environ_main[env_i];
      }
      env_i++;
    }
  }
  else{
    new_environ = malloc(sizeof(char *) * (environ_length+3));
    env_i=0;
    while(*(environ_main+env_i) != '\0'){
      new_environ[env_i] = malloc(sizeof(char) * ENV_VAR_LENGTH);
      new_environ[env_i] = environ_main[env_i];
      puts(new_environ[env_i]);
      env_i++;
    }
     new_environ[env_i] = malloc(sizeof(char) * ENV_VAR_LENGTH);
     strcat(new_environ[env_i],name);
     char *delimiter = "=";
     strcat(new_environ[env_i],delimiter);
     strcat(new_environ[env_i],value);
     env_i++;
  }
    new_environ[env_i] = malloc(sizeof(char) * ENV_VAR_LENGTH);
    new_environ[env_i] = '\0';
    environ_main = new_environ;
  return 0;
}*/