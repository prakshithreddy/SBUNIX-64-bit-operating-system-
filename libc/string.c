#include<string.h>

int strcmp(char *str1,char *str2){
  int i=0;
  while(*(str1+i) != '\0' || *(str2+i) != '\0'){
    /*if(*(str1+i) == '\0')
      return -1;
    if(*(str2+i) == '\0')
      return 1;*/
    if(*(str1+i) == *(str2+i))
      i++;
    else{
      return *(str1+i) - *(str2+i);
    }
  }
  return 0;
}

char *strcpy(char *str1,const char *str2){
  int i=0;
  while((str1[i] = str2[i]) != '\0'){
    i++;
  }
  return str1;
}

void strcat(char *dest, char *src)
{
  (*dest)? strcat(++dest, src): (*dest++ = *src++)? strcat(dest, src): 0 ;
}