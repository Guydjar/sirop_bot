// Version 3.2 du 11/01/16

#ifndef SERVEUR_H_INCLUDED
#define SERVEUR_H_INCLUDED

int server();
void player(int fd, int hit);
void logger_serv(int type, char *s1, char *s2, int socket_fd);





#endif // SERVEUR_H_INCLUDED
