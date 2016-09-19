// Version 3.2 du 11/01/16

#ifndef SEMAPHORE_H_INCLUDED
#define SEMAPHORE_H_INCLUDED

int sem_create (int initval);

int sem_connect ();

void down(int semid, int sem_num);

void up(int semid,int sem_num);

void sem_delete(int semid);

#endif // SEMAPHORE_H_INCLUDED
