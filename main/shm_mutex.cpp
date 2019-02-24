#include "shm_mutex.h"

shm_mutex_t accept_mutex;

int
ngx_trylock_fd(int fd)
{
    // 文件锁
    struct flock  fl;

    bzero(&fl, sizeof(struct flock));
    fl.l_type = F_WRLCK;        // 写锁
    fl.l_whence = SEEK_SET;     /* Seek from beginning of file.  从文件开始处*/

    if (fcntl(fd, F_SETLK, &fl) == -1) {
        return -1;
    }

    return 0;
}

int
ngx_shmtx_trylock(shm_mutex_t *mtx)
{
    int  err;

    err = ngx_trylock_fd(mtx->fd);

    if (err == 0) {
        return 1;
    }

    if (err == EAGAIN) {
        return 0;
    }

    return 0;
}

int
ngx_unlock_fd(int fd){
    struct flock  fl;

    bzero(&fl, sizeof(struct flock));
    fl.l_type = F_UNLCK;
    fl.l_whence = SEEK_SET;

    if (fcntl(fd, F_SETLK, &fl) == -1) {
        return  -1;
    }
    return 0;
}

void
ngx_shmtx_unlock(shm_mutex_t *mtx)
{
    int  err;

    err = ngx_unlock_fd(mtx->fd);

    if (err == 0) {
        return;
    }
}
