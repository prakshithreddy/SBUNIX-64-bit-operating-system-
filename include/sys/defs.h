#ifndef _DEFS_H
#define _DEFS_H

#define NULL ((void*)0)

typedef unsigned long  uint64_t;
typedef          long   int64_t;
typedef unsigned int   uint32_t;
typedef          int    int32_t;
typedef unsigned short uint16_t;
typedef          short  int16_t;
typedef unsigned char   uint8_t;
typedef          char    int8_t;

typedef uint64_t size_t;
typedef int64_t ssize_t;
typedef unsigned short mode_t;
typedef uint64_t off_t;
typedef uint32_t id_t;

typedef uint32_t pid_t;

struct stat {
    unsigned int        st_dev;         /* Device.  */
    unsigned int        st_ino;         /* File serial number.  */
    unsigned int        st_mode;        /* File mode.  */
    unsigned int        st_nlink;       /* Link count.  */
    unsigned int        st_uid;         /* User ID of the file's owner.  */
    unsigned int        st_gid;         /* Group ID of the file's group. */
    unsigned long       st_rdev;        /* Device number, if device.  */
    long                st_size;        /* Size of file, in bytes.  */
    int         st_blksize;     /* Optimal block size for I/O.  */
    long                st_blocks;      /* Number 512-byte blocks allocated. */
    long                st_atime;       /* Time of last access.  */
    long                st_mtime;       /* Time of last modification.  */
    long                st_ctime;       /* Time of last status change.  */
};

#define PROT_READ 0x1
#define PROT_WRITE 0x2
#define PROT_EXEC 0x4
#define PROT_NONE 0x0

#define MAP_SHARED 0x01
#define MAP_PRIVATE     0x02
#define MAP_ANONYMOUS 0x20

#define WNOHANG 1
#define WUNTRACED 2

#define __WAIT_INT(status)      (status)
#define WIFEXITED(status)       __WIFEXITED (__WAIT_INT (status))
# define WIFSIGNALED(status)    __WIFSIGNALED (__WAIT_INT (status))

/* If WIFSIGNALED(STATUS), the terminating signal.  */
#define __WTERMSIG(status)      ((status) & 0x7f)

/* Nonzero if STATUS indicates normal termination.  */
#define __WIFEXITED(status)     (__WTERMSIG(status) == 0)

/* Nonzero if STATUS indicates termination by a signal.  */
#define __WIFSIGNALED(status) \
(((signed char) (((status) & 0x7f) + 1) >> 1) > 0)

#define O_RDONLY        0x0000
#define O_WRONLY        0x0001
#define O_RDWR          0x0002
#define O_CREAT         0x0100
#define O_DIRECTORY     00200000


#define DT_UNKNOWN      0
#define DT_FIFO         1
#define DT_CHR          2
#define DT_DIR          4
#define DT_BLK          6
#define DT_REG          8
#define DT_LNK          10
#define DT_SOCK         12
#define DT_WHT          14

#define P_ALL           0
#define P_PID           1
#define P_PGID          2
#endif
