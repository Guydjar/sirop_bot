// Version 3.4 du 20/10/16

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <math.h>
#include "../Serveur/config.h"
#include "../Serveur/robot_simu.h"
//#include "config.h"
//#include "client.h"


int fd; // socket File descriptor

int init_connexion(char *nom, char *ip,int port)
{
	int err,nbplayer;
	struct sockaddr_in serverAddr;
	char message[BUFSIZE] , server_reply[BUFSIZE];
	sprintf(message,"%s",nom);

	printf("#####################################################\n");
	printf("Bienvenue sur le client version 1 du projet sirop_bot\n");
	printf("#####################################################\n");

	printf("\nConnexion au serveur\n");

	serverAddr.sin_addr.s_addr = inet_addr(ip);
	serverAddr.sin_port = htons(port);
	serverAddr.sin_family = AF_INET;

	// creation de la socket
	if ((fd = socket(AF_INET,SOCK_STREAM,0)) < 0)
	{
		printf("socket error\n");
		return -1;
	}
	// requete de connexion
	if((err = connect(fd,(struct sockaddr *)&serverAddr,sizeof(serverAddr))) < 0 )
	{
		printf("connect error\n");
		return -1;
	}
	// envoie de donne et reception
	if((err = send(fd,message,strlen(message)+1,0))<0)
		printf("send error\n");
	if((err = recv(fd,server_reply,sizeof(server_reply),0))<0)
		printf("receive error\n");
	sscanf(server_reply,"Bonjour %s vous etes le joueur %d",nom,&nbplayer);
	printf("%s\n",server_reply);
	printf("Wait other player\n");
	return 1;
}

// permet de récupérer la position d'un robot:
// pas de parametre
// renvoie une structure s_robot
s_robot get_position()
{
	int err;
	s_robot myrobot;
	char message[BUFSIZE] , server_reply[BUFSIZE];

	sprintf(message,"A;");
	if((err = send(fd,message,strlen(message)+1,0))<0)
		printf("send error\n");
	if((err = recv(fd,server_reply,sizeof(server_reply),0))<0)
		printf("receive error\n");
	sscanf(server_reply,"%f;%f;%f;%d;",&myrobot.x,&myrobot.y,&myrobot.teta,&myrobot.exist);

	return myrobot;
}

// permet de demander de déplacer un robot
// prend en parametre :
//	- la vitesse du robot (speed) en m/s
//	- la direction du robot en radian
// ne renvoie rien
void move(float speed, float teta){
	int err;
	char message[BUFSIZE] , server_reply[BUFSIZE];

	sprintf(message,"B;%f;%f;",speed,teta);
	if((err = send(fd,message,strlen(message)+1,0))<0)
		printf("send error\n");
	if((err = recv(fd,server_reply,sizeof(server_reply),0))<0)
		printf("receive error\n");
}

// permet de connaitre l'etat du robot, pour savoir si il touche un objet (mur, autre robot)
// pas de parametre
// renvoie 1 char (8bits) pour connaitre quel zone est en contact (bit0 pour zone 1 ... bit7 pour zone 8
char get_status(){
	int err;
	char c;
	char message[BUFSIZE] , server_reply[BUFSIZE];

	sprintf(message,"C;");
	if((err = send(fd,message,strlen(message)+1,0))<0)
		printf("send error\n");
	if((err = recv(fd,server_reply,sizeof(server_reply),0))<0)
		printf("receive error\n");
	sscanf(server_reply,"%c;",&c);

	return c;
}

// permet de connaitre le niveau de vie d'un robot
// - pas de parametre
// - renvoie un entier qui représente le % de vie du robot (0-100)
int getlife(){
	int err;
	int ret;
	char message[BUFSIZE] , server_reply[BUFSIZE];

	sprintf(message,"D;");
	if((err = send(fd,message,strlen(message)+1,0))<0)
		printf("send error\n");
	if((err = recv(fd,server_reply,sizeof(server_reply),0))<0)
		printf("receive error\n");
	sscanf(server_reply,"%d;",&ret);

	return ret;
}

// permet au robot de scanner grossierement à 360° les obstacles sur 8 zones et renvoie la distance la plus courte pour chaque zone
// - pas de parametre
// - renvoie un pointeur vers tableau de 8 floats (1 par zone) correspondant à la distance la plus courte
scan_allaround_result scan_allaround(){
	int err;
	scan_allaround_result scan_around;
	char message[BUFSIZE] , server_reply[BUFSIZE];

	sprintf(message,"E;");
	if((err = send(fd,message,strlen(message)+1,0))<0)
		printf("send error\n");
	if((err = recv(fd,server_reply,sizeof(server_reply),0))<0)
		printf("receive error\n");
	sscanf(server_reply,"%f;%f;%f;%f;%f;%f;%f;%f;",&scan_around.zone_distance[0],&scan_around.zone_distance[1],&scan_around.zone_distance[2],&scan_around.zone_distance[3],&scan_around.zone_distance[4],&scan_around.zone_distance[5],&scan_around.zone_distance[6],&scan_around.zone_distance[7]);
	return scan_around;
}

// permet au robot de scanner finement les obstacles sur une zone de 30° (12 zones) et renvoie les 10 objets les plus proches
// prend en parametre:
// 	- le numero de zone à scanner (1 - 12)
// renvoie un pointeur vers la structure scan_zone_result (10 objets les plus proches et leurs types)
// 0 = pas d'objet
// 1 = mur
// 2 = robot ennemi
// 3 = missile
scan_zone_result scan_zone(float teta_scan){
	int err;
	scan_zone_result scan_zone;
	char message[BUFSIZE] , server_reply[BUFSIZE];

	sprintf(message,"F;%f;",teta_scan);
	if((err = send(fd,message,strlen(message)+1,0))<0)
		printf("send error\n");
	if((err = recv(fd,server_reply,sizeof(server_reply),0))<0)
		printf("receive error\n");
	sscanf(server_reply,"%f;%d;",&scan_zone.dist_scan[0],&scan_zone.type_scan[0]);
	return scan_zone;
}

// permet au robot de tirer un missile dans une direction donnée
// prend en parametre:
// 	- l'angle de tir en radian (float)
// 	- ne renvoie rien
void missile_shoot(float teta){
	int err;
	char message[BUFSIZE] , server_reply[BUFSIZE];

	sprintf(message,"G;%f;",teta);
	if((err = send(fd,message,strlen(message)+1,0))<0)
		printf("send error\n");
	if((err = recv(fd,server_reply,sizeof(server_reply),0))<0)
		printf("receive error\n");
}

// permet de connaitre le nombre de missiles tirés non explosés
// pas de parametre
// renvoie :
// 	- un entier qui correspond aux nombres de missiles tirés
int missile_status(){
	int err;
	int ret;
	char message[BUFSIZE] , server_reply[BUFSIZE];

	sprintf(message,"H;");
	if((err = send(fd,message,strlen(message)+1,0))<0)
		printf("send error\n");
	if((err = recv(fd,server_reply,sizeof(server_reply),0))<0)
		printf("receive error\n");
	sscanf(server_reply,"%d;",&ret);
	return ret;
}

// To close connexion
void close_connexion(){
	int err;
	char message[BUFSIZE];
	sprintf(message,"Z;");
	if((err = send(fd,message,strlen(message)+1,0))<0)
		printf("send error\n");
	shutdown(fd,2);
	close(fd);
}
