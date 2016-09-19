// Version 3.2 du 11/01/16

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <arpa/inet.h>

#include "robot_api.h"
#include "config.h"
#include "semaphore.h"

#define VERSION 2
#define ERROR      42
#define LOG        44
#define FORBIDDEN 403
#define NOTFOUND  404

// create the semaphore set to manage access to shared memory
// return  file descriptor on the semaphore set
int sem_create (int initval) {

	int semid ;
	// create semaphore
	semid = semget(CLEFSEM,1,IPC_CREAT|IPC_EXCL|0666);
	if (semid == -1)
	{
		semid = semget(CLEFSEM,1,0666) ;
		if (semid == -1)
		{
			printf("Erreur semget()") ;
			exit(1) ;
		}
	}
	// init semaphore
	semctl(semid,0,SETVAL,initval);
	return semid ;
}

// Create semaphore
// Return semaphore ID
int sem_connect ()
{
	int semid ;
	if((semid = semget(CLEFSEM,1,IPC_CREAT | 0666))<0)
		printf("erreur sem connect");
	return semid ;
}

// the value of the semaphore is incremented by 1 if it is different from 0 if the calling process is blocked and is placed in a queue linked to the semaphore
void down(int semid, int sem_num) {

	struct sembuf sempar ;

	sempar.sem_num = sem_num ;
	sempar.sem_op = -1 ;
	sempar.sem_flg = 0 ;
	if ( semop(semid,&sempar,1) == -1)
		printf("Erreur lors du down") ;
}

//the semaphore value is incremented by 1 if there is no process in the queue otherwise s remains unchanged and releases the first process of the queue
void up(int semid,int sem_num) {

	struct sembuf sempar ;
	sempar.sem_num = sem_num ;
	sempar.sem_op = 1 ;
	sempar.sem_flg = 0 ;

	if (semop(semid,&sempar,1) ==-1)
		printf("Erreur lors du up") ;
}
//Delete semaphore
void sem_delete(int semid){
	if(semctl(semid,0,IPC_RMID,0) == -1)
		printf("Erreur lors de la destruction du semaphore") ;
}
