#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <cerrno>

typedef struct{
    int fd;
} shm_mutex_t;

extern shm_mutex_t accept_mutex;

int
ngx_trylock_fd(int fd);

int
ngx_shmtx_trylock(shm_mutex_t *mtx);

int
ngx_unlock_fd(int fd);

void
ngx_shmtx_unlock(shm_mutex_t *mtx);