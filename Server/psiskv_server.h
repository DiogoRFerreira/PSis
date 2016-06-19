#ifndef psiskv_server_h 
#define psiskv_server_h 

extern pthread_rwlock_t	rwlock, rwlock2;
extern pthread_mutex_t lock;

extern int fd_pipeBackup[2];

#endif /* psiskv_server_h */
