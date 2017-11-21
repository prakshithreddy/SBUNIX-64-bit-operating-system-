#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//#include <sys/mman.h>
#include <string.h>
#include <sys/wait.h>
#include <dirent.h>
//#include <stdbool.h>used for declaring pipe and background flags
#include <fcntl.h>
#define COMMAND_BUFFER 256
#define ARGUMENT_BUFFER 32
#define BG_PIDS_BUFFER 32
#define COMMAND_LENGTH 64
#define PIPE_BUFFER 16
#define LINE_BUFFER 64
#define ENV_VAR_LENGTH 256
//char **environ_main;
int environ_length;
extern char **environ_main;
int *bg_pids;
int bg_count = 0, bg_buffer = BG_PIDS_BUFFER;
short int background = 0;
short int pipe_flag = 0;
char *PS1_SBUSH;

int _puts_(const char *s)
{
  for( ; *s; ++s) if (putchar(*s) != *s) return EOF;
  return 0;
}

char *read_command() {
  int position = 0, buffer = COMMAND_BUFFER;
  char c, *command = (char *)malloc(sizeof(char) * COMMAND_BUFFER);//mmap(NULL,sizeof(char) * COMMAND_BUFFER,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_PRIVATE,-1,0);
  while(1){
    c=getchar();
    //read(0,&c,1);
    //rereif (c == EOF || c=='\n')
    if(c == '\n')
      break;
    if( (position==0) && c ==' ')
      ;
    else if(position < buffer) 
      command[position++] = c;
    else{
      buffer += COMMAND_BUFFER;
      //rerecommand = realloc(command,buffer);
      if(!command){
	//dedeprintf("Error: Memory Allocation failure in sbu shell for the command buffer\n");
        exit(1);
      }
      command[position++] = c;
    }
  }
  //int i = position-1;
  //while((command[i] == ' ' || command[i] == '\t' || command[i] == '\r' || command[i] == '\a') && position != 0){
    //command[i] = NULL;
    //position--;
  //}
  command[position] = '\0';
  //printf("command is:*%s*",command);
  return command;
}

int finder(char* folder_name,char* file_name)
{
    struct dirent* temp;
    
    char* dirp = (char*)malloc(4096);
    
    int fd = open(folder_name,O_RDONLY|O_DIRECTORY);
    
    int n = getdents(fd,dirp,4096);
    
    for( int i=0;i<n;)
    {
        temp = (struct dirent*) (dirp+i);
        
        int d_type = *(dirp + i + temp->d_reclen - 1);
        
        if(d_type == DT_DIR)
        {
            i+=temp->d_reclen;
            continue;
            
        }
        
        if(strcmp(file_name,temp->d_name) == 0)
          return 0;
        //if found 1 else 0
        i+=temp->d_reclen;
    }
  return 1;
}

char *token(char *command, int x,int y){
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

char **parse_command(char *command){
  char **com_args = (char **)malloc(ARGUMENT_BUFFER * sizeof(char *));//mmap(NULL,sizeof(char *) * ARGUMENT_BUFFER,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_PRIVATE,-1,0);
  int buffer = ARGUMENT_BUFFER;
  if(!com_args){
    //dedeprintf("Error: Memory allocation failure in the sbu shell for the argument buffer\n");
    exit(1);
  }
  int i,start,end,n;
  i=start=end=n=0;
  if (*command == '\0')
    return NULL;
  while(*(command+i) != '\0'){
    if(n>buffer){
    buffer = buffer+ARGUMENT_BUFFER; 
    //rerecom_args = realloc(com_args,buffer);
    }
    if(command[i] != ' ' && command[i] != '\t' && command[i] != '\a' && command[i] != '\r' && command[i] != '\n'){
      end = i;
    }
    else{
      if((start>end) && (command[i] == ' ' || command[i] == '\t' || command[i] == '\a' || command[i] == '\r' || command[i] == '\n')){
          start++;        
      }
      else if(end == 0)
        start++;
      else{
        //printf("Tokenizing");
        //printf("Command before tokenizing: %s\n",command);
        if(strcmp(com_args[n++] = token(command,start,end),"|") == 0){
          pipe_flag = 1 ;
          //printf("Pipe flag is set to true");
        }
        start=i+1;
        //printf("After Tokeninzing");
      }
    }
    i++;
  }
  if(start<=end){
    //printf("n before last arg: %d",n);
    //com_args[n] = token(command,start,end);
    //printf("n after com_args: %d, com_args[n] at last = %s",n,com_args[n]);
    if(strcmp(com_args[n] = token(command,start,end),"|") == 0 ){
      pipe_flag = 1;
    }
  }
  else{
    n--;
  }
  //printf("here1");
  if(strcmp(com_args[n],"&") == 0){
    background = 1;
    //printf("here2");
    com_args[n] = '\0';
  }
  else{
    //printf("here3");
    n++;
    com_args[n] = '\0';
  }
  return com_args;
}

int parse_path(char *command,char **com_args){
  //puts(command);
  char **path_args = (char **)malloc(ARGUMENT_BUFFER * sizeof(char *));
  int buffer = ARGUMENT_BUFFER;
  if(!path_args){
    //dedeprintf("Error: Memory allocation failure in the sbu shell for the argument buffer\n");
    exit(1);
  }
  int i,start,end,n;
  i=start=end=n=0;
  if (*command == '\0')
    return 1;
  while(*(command+i) != '\0'){
    if(n>buffer){
    buffer = buffer+ARGUMENT_BUFFER; 
    //rerecom_args = realloc(com_args,buffer);
    }
    if(command[i] != ':'){
      end = i;
    }
    else{
        //puts(token(command,start,end));
        if(finder(path_args[n] = token(command,start,end),com_args[0]) == 0){
          char *delimiter = "/";
          strcat(path_args[n],delimiter);
          strcat(path_args[n],*com_args);
          //puts(path_args[n]);
          if(execve(path_args[n],com_args,environ_main) == -1){
            //dedeprintf("Error while executing the command\n");
            exit(1);//need to find out whether to use _exit or exit
          }
          n++;
        //start=i+1; //printf("After Tokeninzing");
        }
        start = i+1;
    }
    i++;
  }
  if(start<=end){
    if(finder(path_args[n] = token(command,start,end),com_args[0]) == 0 ){
      strcat(path_args[n],com_args[0]);
          if(execve(com_args[0],com_args,environ_main) == -1){
            //dedeprintf("Error while executing the command\n");
            exit(1);//need to find out whether to use _exit or exit
          }
    }
  }
  return 1;
}

int change_directory(char **com_args){
  if(*(com_args+1) == '\0'){
    //dedeprintf("Error: expected argument to \"cd\"\n");
    return -1;
  }
  else{
    if (chdir(com_args[1]) != 0){
      //printf("COM ARGS of 1 is : %s",com_args[1]);
      //dedeprintf("Error executing cd command in sbush\n");
      return -1;
    }
  }
  return 1;
}

int fork_execution(char **com_args,short int background){
  pid_t pid,wait_pid;
  int com_status;
  //bg_pids = malloc(sizeof(int) * BG_PIDS);
  pid = fork();
  
  if(pid == 0){
    if(background){
      close(0);
      //open("/dev/null","r");
    }
    //char **garbage = {'\0'};
    //char *garbage2 = "in fork";
    //write(1,garbage2,7);
    //execve(com_args[0],com_args,garbage);
    int env_i =1;
    while(*(environ_main+env_i) != '\0'){
      int arg_i=0,end=0;
      char *s = malloc(sizeof(char) * COMMAND_BUFFER);
      while(environ_main[env_i][arg_i] != '='){
        end=arg_i;
        arg_i++;
      }
      arg_i++;
      //printf("VAR: %s\n",token(environ[env_i],0,end));
      //puts(environ_main[env_i]);
      if(strcmp(token(environ_main[env_i],0,end),"PATH") == 0){
        int start = arg_i;
        while(environ_main[env_i][arg_i] != '\0'){
          end=arg_i;
          arg_i++;
        }
        if(start<=end)
          s=token(environ_main[env_i],start,end);
          //puts(s);
          parse_path(s,com_args);
      //printf("VAL: %s\n",token(envp[env_i],start,end));
      }
      env_i++;
    }
    if(execve(com_args[0],com_args,environ_main) == -1){
      //dedeprintf("Error while executing the command\n");
      exit(1);//need to find out whether to use _exit or exit
    }
  }
  else if(pid < 0){
    //dedeprintf("Error while forking\n");//its good to use perror instead of printf to print the error.
  }
  else{
    if (background){
      wait_pid = waitpid(pid,&com_status,WNOHANG);
      //dedeprintf("Process pid: %d \n", pid);
      if(bg_count >= bg_buffer){
        bg_buffer = bg_buffer + BG_PIDS_BUFFER;
        //rerebg_pids = realloc(bg_pids,bg_buffer);
      }//need to check if the child process finished or not and print it to the console.
      bg_pids[bg_count++] = pid;
      if(wait_pid == -1){
        //dedeprintf("Error in waitpid\n");
        exit(-1);
      }
    }
    else{
      do{ 
        wait_pid = waitpid(pid,&com_status,WUNTRACED);
        if(wait_pid == -1){
          //dedeprintf("Error in waitpid\n");
          exit(-1);
        }
      }while(!WIFEXITED(com_status) && !WIFSIGNALED(com_status));
    }
  }
  return com_status;
}
int execute_command(char **com_args,short int background){
  //printf("%s\n",com_args[0]);
  //printf("%s\n",com_args[1]);
  if(com_args == NULL){
    //printf("No Command Passed\n");
    return 1;
  }
  int rc = 0;
  if(strcmp(*com_args,"cd") == 0){
  rc = change_directory(com_args);
  }
  else if(strcmp(*com_args,"exit") == 0){
    //int x=0,y=0;
    if(*(com_args+1) == '\0'){
    //printf("Error: expected argument to \"exit\"\n");
    exit(0);
    }
    else{
      int x = 0, y=0;
      while(com_args[1][y] != '\0'){
        if((com_args[1][y] >= '0') && (com_args[1][y] <= '9')){
          x = x * 10 + (com_args[1][y] - '0');
          y++;
        }
        else{
          //dedeprintf("Not a valid return code");
          return -1;
        }
      }
      exit(y);
    }  
  }
  else if(strcmp(*com_args,"export") == 0){
    if(*(com_args+1) == '\0'){
      //dedeprintf("Error: expected argument to set Environmental variable\n");
      return -1;
    }
    else{
      int start=0,end=0,i=0;
      char **exp_args = (char **)malloc(sizeof(char *) * 4);//mmap(NULL,sizeof(char *) * 4,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_PRIVATE,-1,0);
      exp_args[0] = "setenv";
      while(com_args[1][i] != '='){
        if(com_args[1][i] == '\0'){
          //dedeprintf("Error while setting environment varibale, expecting a var and a value\n");
          return -1;
        }
        end=i;
        i++; 
      }
      exp_args[1] = token(com_args[1],start,end);
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
        exp_args[2] = token(com_args[1],start,end);
      else{
        //dedeprintf("Error while setting environment varibale, expecting a value\n");
        return -1;
      }
      *(exp_args+3) = '\0';
      if(strcmp(exp_args[1],"PS1") == 0){
        strcpy(PS1_SBUSH,exp_args[2]);
        exp_args[1] = "PS1_SBUSH";
      }
      if(setenv(exp_args[1],exp_args[2],1) == 0){
        /*char *s=getenv(exp_args[1]);
        printf("**%s**",s);*/
        return 0;
      }  
      else{
        return -1;
      }  
    }
  }
  else{
    rc =fork_execution(com_args,background);
  }
  return rc;
}

int pipe_execution(char **com_args,short int background){
  int i=0;//buffer1 = ARGUMENT_BUFFER,j=0,buffer2 = ARGUMENT_BUFFER;
  //char **command1;
  //char **command2;
  char ***commands = (char ***)malloc(sizeof(char **) * PIPE_BUFFER);//mmap(NULL,sizeof(char **) * PIPE_BUFFER,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_PRIVATE,-1,0);
  int  *arg_buffers = (int *)malloc(sizeof(int *) * PIPE_BUFFER);//mmap(NULL,sizeof(int) * COMMAND_BUFFER,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_PRIVATE,-1,0);
  //int buffer_p = PIPE_BUFFER;//need to use this for allocating buffer to pipes automatically
  int pipe_count=0;
  /*//printf("Inside Pipe Execution");
  command1 = malloc(sizeof(char *) * ARGUMENT_BUFFER);
  while(strcmp(com_args[i],"|") != 0){
    if(i>=buffer1){
      buffer1 = buffer1 + ARGUMENT_BUFFER;
      command1 = realloc(command1,buffer1);
    }
    command1[i] = malloc(sizeof(char) * COMMAND_LENGTH); // need to implement realloc if command argument is greater than COMMAND_LENGTH
    strcpy(command1[i],com_args[i]);
    //printf("command1: %s",command1[i]);
    i++;
  }
  command1[i] = malloc(sizeof(char) * 2);
  command1[i] = '\0';
  i++;
  //printf("i before command2 %d and com_args before command2 *%s*",i,com_args[i]);
  command2 = malloc(sizeof(char *) * ARGUMENT_BUFFER);*/
  while(*(com_args+i) != '\0'){
    commands[pipe_count] = (char **)malloc(sizeof(char *) * ARGUMENT_BUFFER);//mmap(NULL,sizeof(char *) * ARGUMENT_BUFFER,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_PRIVATE,-1,0);
    int arg_count = 0;
    arg_buffers[pipe_count] = ARGUMENT_BUFFER;
    while((strcmp(com_args[i],"|") != 0) && (*(com_args+i) != '\0')){
      if(arg_count>=arg_buffers[pipe_count]){
        arg_buffers[pipe_count] = arg_buffers[pipe_count] + ARGUMENT_BUFFER;
        //rerecommands[pipe_count] = realloc(commands[pipe_count],arg_buffers[pipe_count]);
      }
      commands[pipe_count][arg_count] = (char *)malloc(sizeof(char) * COMMAND_LENGTH);//mmap(NULL,sizeof(char) * COMMAND_LENGTH,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_PRIVATE,-1,0);//need to implement realloc if command argument is greater than COMMAND_LENGTH
      strcpy(commands[pipe_count][arg_count],com_args[i]);
      //printf("Command arguments: *%s*",commands[pipe_count][arg_count]);
      i++;
      arg_count++;
      if(*(com_args+i) == '\0'){
        i--;
        break;
      }
    }
    commands[pipe_count][arg_count] = (char *)malloc(sizeof(char) * 2);//mmap(NULL,sizeof(char) * 2,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_PRIVATE,-1,0);
    commands[pipe_count][arg_count] = '\0';
    pipe_count++;
    i++;
  }
  //command2[j] = malloc(sizeof(char) * 2);
  //command2[j] = '\0';
  //free(command2);
  //free(command1);
  int fd[pipe_count][2];
  int pipes = 0;
  while(pipes < pipe_count){
    if(pipe(fd[pipes]) == -1){
      //dedeprintf("Error while creating a pipe");
      exit(1);
    }
    pipes++;
  }
  /*if(pipe(fd) == -1){
    //creating pipe which creates stdin in fd0 and stdout in fd1, watever is written in fd1 will be input to fd0
    perror("Error while creating a pipe");
    _exit(1);
  }*/
  pid_t pid[pipe_count],wait_pid[pipe_count];
  int com_status[pipe_count];
  int c_count=0,fd_count =0;
  while(c_count < pipe_count){
    pid[c_count] = fork();
    if(pid[c_count] < 0){
      //dedeprintf("Error while forking 1");
      exit(1);
    }
    if(pid[c_count] == 0){
      //free(command2);//not sure if we canfree the command2 in child process of command1
      if(c_count == (pipe_count -1)){
        close(0);
        if(dup(fd[c_count-1][0]) == -1){
          //dedeprintf("Error while duping");
          exit(1);
        //close(0);
        }
      }
      if(c_count == 0){
        close(1);
        if(dup(fd[c_count][1]) == -1){
          //dedeprintf("Error while duping");
          exit(1);
        }
      }
      else if((c_count > 0) && c_count < (pipe_count-1)){
        close(0);
        if(dup(fd[c_count-1][0]) == -1){
          //dedeprintf("Error while duping");
          exit(1);
        }
        close(1);
        if(dup(fd[c_count][1]) == -1){
          //dedeprintf("Error while duping");
          exit(1);
        }
      }
      while(fd_count < pipe_count){
        close(fd[fd_count][0]);
        close(fd[fd_count][1]);
        fd_count++;
      }
      fd_count =0;
      int env_i =1;
      while(*(environ_main+env_i) != '\0'){
        int arg_i=0,end=0;
        char *s = malloc(sizeof(char) * COMMAND_BUFFER);
        while(environ_main[env_i][arg_i] != '='){
          end=arg_i;
          arg_i++;
        }
        arg_i++;
      //printf("VAR: %s\n",token(environ[env_i],0,end));
      //puts(environ_main[env_i]);
        if(strcmp(token(environ_main[env_i],0,end),"PATH") == 0){
         int start = arg_i;
          while(environ_main[env_i][arg_i] != '\0'){
            end=arg_i;
            arg_i++;
          }
          if(start<=end){
            s=token(environ_main[env_i],start,end);
            //puts(s);
            parse_path(s,commands[c_count]);
          }
      //printf("VAL: %s\n",token(envp[env_i],start,end));
        }
        env_i++;
      }
      if(execve(commands[c_count][0],commands[c_count],environ_main) == -1){
        //dedeprintf("Error while executing command");
        exit(1);
      }
    }
    c_count++;
  }
  /*pid1 = fork();
  if(pid1 < 0){
    perror("Error while forking 1");
    _exit(1);
  }
  if(pid1 == 0){
    //free(command2);//not sure if we canfree the command2 in child process of command1
    close(1);
    if(dup(fd[1]) == -1){
      perror("Error while duping in 1");
      //free(command);
      //free(com_args);
      _exit(1); 
    }
    close(fd[0]);
    close(fd[1]);
    if(execvp(command1[0],command1) == -1){
      perror("Error while executing command 1");
      _exit(1);
    }   
  }

  pid2 = fork();
  if(pid2 < 0){
    perror("Error while forking 2");
    _exit(1);
  }
  if(pid2 == 0){
    //free(command1); //not sure if we can free the command1 in child process of command2
    close(0);
    if(dup(fd[0]) == -1){
      perror("Error while duping in 2");
      //free(command);
      //free(com_args);
      _exit(1);
    }
    close(fd[0]);
    close(fd[1]);
    if(execvp(command2[0],command2) == -1){
      perror("Error while executing command 2");
      _exit(1);
    }
  }
  */
  while(fd_count < pipe_count){
    close(fd[fd_count][0]);
    close(fd[fd_count][1]);
    fd_count++;
  }
  fd_count = 0;
  //if(pid1 > 0 && pid2 >0){
    //close(fd[0]);
    //close(fd[1]);
    //sleep(1);
    //free(command1);
    //free(command2);
  if (background){
    while(fd_count < pipe_count){
      wait_pid[fd_count] = waitpid(pid[fd_count],&com_status[fd_count],WNOHANG);
      if((wait_pid[fd_count] == -1)){
        //dedeprintf("Error in waitpid of pipe while running command");
        exit(-1);
      }
      if(bg_count >= bg_buffer){
        bg_buffer = bg_buffer + BG_PIDS_BUFFER;
        //rerebg_pids = realloc(bg_pids,bg_buffer);
      }//need to check if the child process finished or not and print it to the console.
      bg_pids[bg_count++] = pid[fd_count];
      fd_count++;
      }
      //wait_pid2 = waitpid(pid1,&com2_status,WNOHANG);
      //dedeprintf("Process pid: %d \n", pid[fd_count-1]);
      /*if(bg_count >= bg_buffer){
        bg_buffer = bg_buffer + BG_PIDS_BUFFER;
        bg_pids = realloc(bg_pids,bg_buffer);
      }//need to check if the child process finished or not and print it to the console.
      bg_pids[bg_count++] = pid2;
      bg_pids[bg_count++] = pid1;
      if((wait_pid1 == -1)){
        perror("Error in waitpid of pipe while running command1");
        exit(EXIT_FAILURE);
      }
      if((wait_pid2 == -1)){
        perror("Error in waitpid of pipe while running command2");
        exit(EXIT_FAILURE);
      }*/
    }
    else{
      while(fd_count < pipe_count){
        do{
          wait_pid[fd_count] = waitpid(pid[fd_count],&com_status[fd_count],WUNTRACED);
          if(wait_pid[fd_count] == -1){
            //dedeprintf("Error in waitpid1 in pipe while running command1");
            exit(-1);
          }
        }while(!WIFEXITED(com_status[fd_count]) && !WIFSIGNALED(com_status[fd_count]));
        fd_count++;
      /*do{
        wait_pid2 = waitpid(pid2,&com2_status,WUNTRACED);
        if(wait_pid2 == -1){
          perror("Error in waitpid2 in pipe while running command2");
          exit(EXIT_FAILURE);
        }
      }while(!WIFEXITED(com2_status) && !WIFSIGNALED(com2_status));*/
      }
      free(commands);//munmap(commands,sizeof(char **) * PIPE_BUFFER);
      //free(command2);
      return com_status[fd_count -1];
    }
  return 0;
}
char *read_line(int fd){
  char *d = (char *)malloc(sizeof(char) * LINE_BUFFER);//mmap(NULL,sizeof(char) * LINE_BUFFER,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_PRIVATE,-1,0);
  char *c = (char *)malloc(sizeof(char) * 1);//mmap(NULL,sizeof(char),PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_PRIVATE,-1,0);
  int char_count = 0,char_buffer = LINE_BUFFER,rc;
  //int i=50;
  while(1){
    if(char_count >= char_buffer){
      char_buffer = char_buffer + LINE_BUFFER;
      //rered=realloc(c,char_buffer);
    }
    if((rc =read(fd,c,1)) == -1){
      //dedeprintf("Error while reading file");
      exit(-1);
    }
    else if(rc>0){
      if(!(c[0] == '\n')){
        d[char_count] = c[0];
        char_count++;
      }
      else{
        d[char_count] = '\0';
        return d;
      }
    }
    else if(rc == 0){
      if(char_count > 0){
        d[char_count] = '\0';
        return d;
      }
      else
        return NULL;
    }
    //i--;
  }
  return d;
}

int main(int argc, char *argv[],char *envp[]) {
  /*int i =0;
  printf("%d\n",argc);
  while(*(argv+i) != '\0'){
    printf("%s\n",*(argv+i));
    i++;
  }*/
  //printf("ENVIRON 0: *%s*\n",envp[0]);
  //write(1,envp[1],20);
  //puts(envp[0]);
  //printf("ENVIRON 6: *%s*\n",envp[6]);
  //int argx =1;//nn
  //argc = 1;//nn
  //char *prompt="sbush";
  //write(1,prompt,5);
  //argv = NULL;//nn
  //envp = NULL;//nn
  //char *gg = "Environ not null";
  //int x = sizeof(environ);
  //char c = x + '0';
  //puts(&c);
  //if(c == '8')
  //if(strcmp(environ[0],"NULL") == 0)
  /*if (environ == NULL)
    environ_main = envp;
  else*/
  envp++;
  environ_main = envp;
  //puts(environ_main[0]);
  if(argc >= 2){
    int fp = open(argv[1],O_RDONLY);
    char *line = read_line(fp);
    while((line = read_line(fp)) != NULL){ 
      //printf("LINE: *%s*\n",line);
      char **com_args = parse_command(line);
      if(pipe_flag == 1){
        pipe_execution(com_args,background);
      }
      else
        execute_command(com_args,background);
      free(com_args);//munmap(com_args,sizeof(char *) * ARGUMENT_BUFFER);
      background = 0;
      pipe_flag = 0;
    }
    return 0;
  }

  /*char *p = malloc(sizeof(char) * 10);
  p[0] = 'H';
  write(1,p,1);
  free(p);
  exit(0);*/
  
  PS1_SBUSH = (char *) malloc(sizeof(char) * COMMAND_BUFFER);//mmap(NULL,sizeof(char) * COMMAND_BUFFER,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_PRIVATE,-1,0);
  //exit(0);
  int env_i =1;
  while(*(environ_main+env_i) != '\0'){
    int arg_i=0,end=0;
    while(environ_main[env_i][arg_i] != '='){
      end=arg_i;
      arg_i++;
    }
    arg_i++;
    //printf("VAR: %s\n",token(environ[env_i],0,end));
    //puts(environ[env_i]);
    if(strcmp(token(environ_main[env_i],0,end),"PS1_SBUSH") == 0){
      int start = arg_i;
      while(environ_main[env_i][arg_i] != '\0'){
        end=arg_i;
        arg_i++;
      }
      if(start<=end)
        PS1_SBUSH=token(environ_main[env_i],start,end);
      else
        PS1_SBUSH[0] = '\0';
      //printf("VAL: %s\n",token(envp[env_i],start,end));
    }
    env_i++;
  }
  //PS1_SBUSH[0] = '\0';//nn
  bg_pids = (int *)malloc(sizeof(int) * BG_PIDS_BUFFER);//mmap(NULL,sizeof(int) * BG_PIDS_BUFFER,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_PRIVATE,-1,0);
  while(argc == 1){
    if(PS1_SBUSH[0] == '\0'){
      //printf("\nsbush>");
      char *prompt="\nsbush>";
      _puts_(prompt);
      //int g =write(1,prompt,7);
      //g++;
    }
    else{
      //printf("%s>",PS1_SBUSH);
      _puts_(PS1_SBUSH);
      _puts_(">");
      //int g =write(1,PS1_SBUSH,6);
      //g++;
    }
    char *command = read_command();
    if(*command == '\0'){
      free(command);
      continue;
    }
    char **com_args = parse_command(command);
    //int rc = 
    //close(1);
    if(pipe_flag == 1){
      pipe_execution(com_args,background);
      ;
    }
    else{
      execute_command(com_args,background);
    }
    //char ch;
    //while ((ch = getchar()) != '\n' && ch != EOF)
    //  printf("%c",ch);
    //return rc;
    free(command);//munmap(command,sizeof(char) * COMMAND_BUFFER);
    free(com_args);//munmap(com_args,sizeof(char *) * ARGUMENT_BUFFER);
    background = 0;//resetting this background flag for next command
    pipe_flag = 0;//resetting this pipe flag for next command
    //break;
  }
  free(bg_pids);//munmap(bg_pids,sizeof(int) * BG_PIDS_BUFFER);
  return 0;
}
