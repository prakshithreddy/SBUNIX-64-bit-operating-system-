#ifndef _STDLIB_H
#define _STDLIB_H
#include<sys/defs.h>


//int main(int val);
int main(int argc, char *argv[], char *envp[]);

void exit(int status);
void *malloc(ssize_t size);
void free(void *ptr);
int64_t setenv(char *name,char *value, int overwrite);
char *export(char **com_args);
char *getenv(char *name);
int64_t env(char **com_args);

#endif
