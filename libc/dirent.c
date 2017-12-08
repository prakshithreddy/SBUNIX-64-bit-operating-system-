#include<dirent.h>
#include<sys/syscall.h>

DIR *opendir(const char *name){
    return (DIR *)syscall((void*)9,(void*)name,0,0,0,0,0);
}
struct dirent *readdir(DIR *dirp){
    return (struct dirent *)syscall((void*)11,(void*)dirp,0,0,0,0,0);
    
}
int64_t closedir(DIR *dirp){

    return (int64_t)syscall((void*)10,(void*)dirp,0,0,0,0,0);
}

int getdents(unsigned int fd, char *dirp,unsigned int count){
    
    return (ssize_t)syscall((void*)12,(void*)(ssize_t)fd,(void*)dirp,(void*)(ssize_t)count,0,0,0);
}

/*int chdir(const char *path)
{
  return (ssize_t)syscall((void*)80,(void*)path,0,0,0,0,0);
    
}*/