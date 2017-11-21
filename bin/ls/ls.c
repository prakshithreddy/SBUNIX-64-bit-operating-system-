#include <dirent.h>
#include <unistd.h>
#include <sys/defs.h>
#include<sys/mman.h>
#include<fcntl.h>



int main(int argc,char* argv[],char* envp[])
{
    
        char* file_path;
    
    if(argc==1){
    
        file_path = (char*)mmap(NULL,4096,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_PRIVATE,-1,0);
        getcwd(file_path,1024);
        
    
    }
    else file_path = argv[1];
    
    struct dirent* temp;
    
    char* dirp = (char*)mmap(NULL,4096,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_PRIVATE,-1,0);
    
    int fd = open(file_path,O_RDONLY|O_DIRECTORY);
    
    int n = getdents(fd,dirp,4096);
    
    for( int i=0;i<n;)
    {
        temp = (struct dirent*) (dirp+i);

        
        if(temp->d_name[0]=='.') {
            i+=temp->d_reclen; continue;
        }
        
        int k=0;
        
        while(temp->d_name[k]!='\0')
        {
            k++;
        }
        
        write(1,temp->d_name,k);
        
        int d_type = *(dirp + i + temp->d_reclen - 1);
        
        if(d_type == DT_DIR)
        {
            write(1,"/",1);
            
        }
        
        write(1,"  ",1);
        
        
        i+=temp->d_reclen;
    }
    write(1,"\n",1);
    
    //struct stat* temp = (struct stat*) mmap(NULL,sizeof(struct stat),PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_PRIVATE,-1,0);
    
    // stat(path,temp);
    
    
    
    return 0;
    
    
}
