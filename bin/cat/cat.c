#include<unistd.h>
#include<sys/mman.h>
#include<stat.h>
#include<fcntl.h>



int main(int argc, char*argv[],char* envp[])
{
    
    struct stat* temp = (struct stat*) mmap(NULL,sizeof(struct stat),PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_PRIVATE,-1,0);
    
    stat(argv[1],temp);
    
    int fd = open(argv[1], O_RDONLY);
    
    char* cat = (char*) mmap(NULL,temp->st_size*4096,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_PRIVATE,-1,0);
    
    int l_cou = read(fd,cat,temp->st_size*4096);
    
    write(1,cat,l_cou);
    
    write(1,"\n",1);
    
    return 0;
}
