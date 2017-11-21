#include<unistd.h>

char getchar()
{
  char c;
  read(0,&c,1);
  return c;
}

/*int puts(char *s)
{ 
  char *temp = s;
  while(*temp != '\0'){
    write(1,temp,1);
    temp++;
  }
  return 0;
}*/