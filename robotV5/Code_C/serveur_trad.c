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
#include <sys/time.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "robot_api.h"
#include "config.h"
#include "semaphore.h"
#include "serveur.h"

#define VERSION 2
#define ERROR      42
#define LOG        44
#define FORBIDDEN 403
#define NOTFOUND  404
#define STDIN 0  /* file description for standard input */

int main(int argc, char **argv)
{
	if( argc < 2  || argc > 2 || !strcmp(argv[1], "-?") ) {
		printf("server is a small and very safe mini server for sirop_bot project\n");
		printf("Use ./serveur <port>\n");
		printf("\tVersion : %d\n", VERSION);
		exit(0);
	}

	int i,  pid, listenfd, socketfd, hit,game_status = 0;
	int port;
	socklen_t length;

	struct timeval tv;
	fd_set readfds;



	static struct sockaddr_in cli_addr; 		// static = initialised to zeros
	static struct sockaddr_in serv_addr; 		// static = initialised to zeros
	char client_ip_address[15];

	// Become deamon + unstopable and no zombies children (= no wait())
	if(fork() != 0)
		return 0; 					// parent returns OK to shell
	(void)signal(SIGCLD, SIG_IGN); 	// ignore child death
	(void)signal(SIGHUP, SIG_IGN); 	// ignore terminal hangups
	for(i=0;i<32;i++)
		(void)close(i);				// close open files
	(void)setpgrp();				// break away from process group
	//logger_serv(LOG,"server starting",argv[1],getpid());

	// shared memory
	void* ptr_mem_partagee; 						//	address attachment of partaged memory segment
	structure_partagee Data;						//	instantiate the structure and called Data. Just find the size of structure.

	int mem_ID; 									//	identification of partaged segment memory associated to CLEF.
	int sem_ID; 									//	identification of the organisation of semaphore associated to CLEFSEM

	sem_ID = sem_connect(); 						// recuperation of partaged semaphore
	logger_serv(LOG,"Semaphore  :","ok",111);

	if ((mem_ID = shmget(CLEF, sizeof(Data), 0666)) < 0)			// Find the memory segment of associated CLEF and check the identificator of this one... Attribute the read access only.
		logger_serv(ERROR, "system call","shmget",111);
	if ((ptr_mem_partagee = shmat(mem_ID, NULL, 0)) == (void*) -1)	//	Fix the memory partaged segment identified by mem_ID_B to the data segment of processus B in free zone of operating system
		logger_serv(ERROR, "system call","shmat",111);
	logger_serv(LOG,"shared memory  :","ok",111);

	port = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);

	// setup the network socket
	if((listenfd = socket(AF_INET, SOCK_STREAM,0)) <0)
		logger_serv(ERROR, "system call","socket",0);

	if(bind(listenfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr)) <0)
		logger_serv(ERROR,"system call","bind",0);

	if( listen(listenfd,NB_ROBOT_MAX) <0)
		logger_serv(ERROR,"system call","listen",0);


	for(hit=1;hit<NB_ROBOT_MAX+1;hit++)
	{
		/* ignore writefds and exceptfds: */
		while(1)
		{
			tv.tv_sec = 2;
			tv.tv_usec = 500000;
			FD_ZERO(&readfds);
			FD_SET(listenfd, &readfds);
			select(listenfd+1, &readfds, NULL, NULL, &tv);
			if (FD_ISSET(listenfd, &readfds))
			{
				logger_serv(LOG,"CONNEXION :","TEST",0);
				break;
			}
			down(sem_ID,0);  							// lock the access of partaged memory with the semaphore
			game_status = ((structure_partagee*)ptr_mem_partagee)->game_status;
			up(sem_ID,0);


			if (game_status==1)
			{
				logger_serv(LOG,"quit :","TEST",0);
				shmdt(ptr_mem_partagee);
				(void)close(listenfd);
				return 0;
			}
       }
		length = sizeof(cli_addr);
		if((socketfd = accept(listenfd, (struct sockaddr *)&cli_addr, &length)) < 0)
			logger_serv(ERROR,"system call","accept",0);

		// collect the ip addresse of client
		sprintf(client_ip_address,"%d.%d.%d.%d",
		(int)(cli_addr.sin_addr.s_addr&0xFF),
		(int)((cli_addr.sin_addr.s_addr&0xFF00)>>8),
		(int)((cli_addr.sin_addr.s_addr&0xFF0000)>>16),
		(int)((cli_addr.sin_addr.s_addr&0xFF000000)>>24));
		logger_serv(LOG,"nouvelle connexion :",client_ip_address,0);

		if((pid = fork()) < 0)
			logger_serv(ERROR,"system call","fork",0);
		else
		{
			if(pid == 0) 							// child
			{
				(void)close(listenfd);
				logger_serv(LOG,"n :","fork :",pid);
				player(socketfd,hit); 				// never returns
			}
			else 									// parent
				(void)close(socketfd);
		}

	}
	shmdt(ptr_mem_partagee);
	return 0;
}

void player(int fd, int hit)
{
	int err;
	char message[BUFSIZE] , client_reply[BUFSIZE];

	// Declare variables to receive variables of the structure "structure_partagee" defined in sequencer processus
	int copy_flag_w = 0;
	int copy_flag_r = 0;

	void* ptr_mem_partagee; 						//	address of partaged segment memory
	structure_partagee Data;						//	instantiate the structure and called Data. Just find the size of structure.

	int mem_ID; 									//	identification of partaged segment memory associated to CLEF.
	int sem_ID; 									//	identification of the organisation of semaphore associated to CLEFSEM

	sem_ID = sem_connect(); 						// recuperation of partaged semaphore
	logger_serv(LOG,"Semaphore  :","ok",hit);

	if ((mem_ID = shmget(CLEF, sizeof(Data), 0666)) < 0)			//	Find the memory segment of associated CLEF and check the identificator of this one... Attribute the read access only.
		logger_serv(ERROR, "system call","shmget",0);
	if ((ptr_mem_partagee = shmat(mem_ID, NULL, 0)) == (void*) -1)	//	Fix the memory partaged segment identified by mem_ID_B to the data segment of processus B in free zone of operating system
		logger_serv(ERROR, "system call","shmat",0);
	logger_serv(LOG,"shared memory  :","ok",hit);

	// start communication with client
	if ((err = recv(fd,client_reply,sizeof(client_reply),0))<0)		// read client request in one go
		logger_serv(LOG,"receive error",client_reply,err);

	down(sem_ID,0);  							// lock the partaged memory with semaphore
		sprintf(((structure_partagee*)ptr_mem_partagee)->name[hit-1],"%s",client_reply);
		((structure_partagee*)ptr_mem_partagee)->connected[hit-1] = 1;
	up(sem_ID,0);

	sprintf(message,"Bonjour %s vous etes le joueur %d",client_reply,hit);

	if ((err = send(fd,message,strlen(message)+1,0))<1)
		logger_serv(LOG,"send error",message,hit);

	while(1)
	{
		down(sem_ID,0);  							// lock the partaged memory with semaphore
		copy_flag_r = ((structure_partagee*)ptr_mem_partagee)->flag_r[hit-1];
		copy_flag_w = ((structure_partagee*)ptr_mem_partagee)->flag_w[hit-1];
		up(sem_ID,0); 								// release the semaphore

		// Send function called by client to simulator
		if(0 == copy_flag_r && 0 == copy_flag_w)
		{
			logger_serv(LOG,"if recv :","ok",hit);

			if ((err = recv(fd,client_reply,sizeof(client_reply),0))<0) 	// read client request in one go
				logger_serv(LOG,"receive error",client_reply,hit);
			else{
				down(sem_ID,0); 					// lock the partaged memory with semaphore
				sprintf(((structure_partagee*)ptr_mem_partagee)->tab_cmd[hit-1],"%s",client_reply);
				logger_serv(LOG,"receive  :",((structure_partagee*)ptr_mem_partagee)->tab_cmd[hit-1],hit);
				((structure_partagee*)ptr_mem_partagee)->flag_w[hit-1] = 1;
				up(sem_ID,0);  						// release the semaphore
				if (client_reply[0] == 'Z'){ 		// Close connexion by the client
					shmdt(ptr_mem_partagee);
					close(fd);
					exit(1);
				}
			}
		}
		// Send the answer of function called by client
		if (1 == copy_flag_r && 0 == copy_flag_w)
		{
			logger_serv(LOG,"if send :","ok",hit);
			sprintf(message,"%s",((structure_partagee*)ptr_mem_partagee)->tab_cmd[hit-1]);
			if ((err = send(fd,message,strlen(message)+1,0))<1)
				logger_serv(LOG,"send error",message,hit);
			else
			{
				down(sem_ID,0); 					// Lock access of partaged memory with semaphore
				((structure_partagee*)ptr_mem_partagee)->flag_r[hit-1] = 0;
				logger_serv(LOG,"send  :",((structure_partagee*)ptr_mem_partagee)->tab_cmd[hit-1],hit);
				up(sem_ID,0);						// release the semaphore
			}
		}
	}

	//	Delete the segment ( only when no processus is linked to segment)
	shmdt(ptr_mem_partagee);
	close(fd);
	exit(1);
}

void logger_serv(int type, char *s1, char *s2, int socket_fd)
{
	int fd ;
	char logbuffer[BUFSIZE*2];

	switch (type)
	{
		case ERROR: (
			void)sprintf(logbuffer,"ERROR: %s:%s Errno=%d exiting pid=%d",s1, s2, errno,getpid());
			break;
		case LOG:
			(void)sprintf(logbuffer," INFO: %s:%s:%d",s1, s2,socket_fd);
			break;
	}
	// No checks here, nothing can be done with a failure anyway
	if((fd = open("serveur.log", O_CREAT| O_WRONLY | O_APPEND,0644)) >= 0)
	{
		(void)write(fd,logbuffer,strlen(logbuffer));
		(void)write(fd,"\n",1);
		(void)close(fd);
	}
	if(type == ERROR)
		exit(1);
}





