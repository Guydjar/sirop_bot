/**
 * \file serveur.h
 * \brief Programme de tests.
 * \author Toute l'équipe FP25
 * \version 3.3
 * \date 15 janvier 2016
 *
 * declaration of 2 functions used for connexion between server and client.
 *
 */

#ifndef SERVEUR_H_INCLUDED
#define SERVEUR_H_INCLUDED

/**
*\fn playerError type : ERROR or LOG
*\brief function to communicate between client and simulator
* comments:
* read each client request, validate or not for access
*\param fd file descriptor
*\param hit number of semaphore or shared memory
*/
void player(int fd, int hit);


/**
*\fn logger_serv
*\brief function to write in log file
* comments:
* Error type : ERROR or LOG
*\param type Error type ERROR or LOG
*\param *s1 characters chain for log file and information
*\param *s2 characters chain for log file and information
*\param socket_fd socket file descriptor
*/
void logger_serv(int type, char *s1, char *s2, int socket_fd);





#endif // SERVEUR_H_INCLUDED
