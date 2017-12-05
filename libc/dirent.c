#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/defs.h>
#include <sys/syscall.h>
#include<sys/mman.h>

/*DIR *opendir(const char *name)
{
    int fd = open(name,O_RDONLY);
    
    DIR* dir = (DIR*)mmap(NULL,sizeof(DIR), PROT_WRITE|PROT_READ , (MAP_PRIVATE|MAP_ANONYMOUS),-1,0);
    
    dir->fd = fd;
    
    return dir;
    
}*/
int chdir(const char *path)
{
    
  return (ssize_t)syscall5((void*)80,(void*)path,0,0,0,0);
    
    
}
void list_all_dir(const char* path)
{
    
    struct dirent* temp;
    
    char* dirp = (char*)mmap(NULL,4096,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_PRIVATE,-1,0);
    
    int fd = open(path,O_RDONLY|O_DIRECTORY);
    
     int n = getdents(fd,dirp,4096);
    
    for( int i=0;i<n;)
    {
        temp = (struct dirent*) (dirp+i);
        int k=0;
        int no_print = 0;
        while(temp->d_name[k]!='\0')
        {
            if(temp->d_name[k]=='.') {
                no_print =1 ;break;
            }
            write(1,&(temp->d_name[k]),1);
            k++;
        }
        
        int d_type = *(dirp + i + temp->d_reclen - 1);
        
        if(d_type == DT_DIR && no_print == 0)
        {
            write(1,"/",1);

        }
        
        if(no_print==0)write(1,"  ",1);
        
        i+=temp->d_reclen;
    }
    write(1,"\n",1);
    
    //struct stat* temp = (struct stat*) mmap(NULL,sizeof(struct stat),PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_PRIVATE,-1,0);
    
   // stat(path,temp);
    
    
    
    
    
    
}

int getdents(unsigned int fd, char *dirp,unsigned int count)
{
    return (ssize_t)syscall5((void*)78,(void*)(ssize_t)fd,(void*)dirp,(void*)(ssize_t)count,0,0);
}
