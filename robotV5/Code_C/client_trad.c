// Version 3.2 du 11/01/16

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <math.h>
#include "robot_api.h"
#include "config.h"
#include "client.h"

int fd; // socket File descriptor

int init_connexion(char *nom, char *ip,int port)
{
	int err,nbplayer;
	struct sockaddr_in serverAddr;
	char message[BUFSIZE] , server_reply[BUFSIZE];
	sprintf(message,"%s",nom);

	printf("\nConnexion au serveur\n");

	serverAddr.sin_addr.s_addr = inet_addr(ip);
	serverAddr.sin_port = htons(port);
	serverAddr.sin_family = AF_INET;

	// socket creation
	if ((fd = socket(AF_INET,SOCK_STREAM,0)) < 0)
	{
		printf("socket error\n");
		return -1;
	}
	// request connexion
	if((err = connect(fd,(struct sockaddr *)&serverAddr,sizeof(serverAddr))) < 0 )
	{
		printf("connect error\n");
		return -1;
	}
	// send and receive data
	if((err = send(fd,message,strlen(message)+1,0))<0)
		printf("send error\n");
	if((err = recv(fd,server_reply,sizeof(server_reply),0))<0)
		printf("receive error\n");
	sscanf(server_reply,"Bonjour %s vous etes le joueur %d",nom,&nbplayer);
	printf("%s\n",server_reply);
	printf("Wait other player\n");
	return 1;
}

// get robot position
// no parameters
// return a structure s_robot
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

// ask to move robot
// take parameters:
//	- speed of robot in m/s
//	- direction of robot in radian
// return nothing
void move(float speed, float teta){
	int err;
	char message[BUFSIZE] , server_reply[BUFSIZE];

	sprintf(message,"B;%f;%f;",speed,teta);
	if((err = send(fd,message,strlen(message)+1,0))<0)
		printf("send error\n");
	if((err = recv(fd,server_reply,sizeof(server_reply),0))<0)
		printf("receive error\n");
}

// check state of robot to know if he touch an object (wall, another robot)
// no parameters
// return 1 character (8bits) to know zone in contact (bit0 for zone 1 ... bit7 for zone 8
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

// check the life level of robot
// - no parameters
// - return an integer wich represente % of robot life (0-100)
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

// global scan of robot at 360° for barrier in 8 zones and return the most short distance
// - no parameters
// - return the pointer to array 8 floats (1 per zone) corresponding to the shorter distance
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

// thin scanning robot to check obstacles in zone of 30° (12 zones) and return the closer 10 objects
// parameter:
// 	- number of zone to scan (1 - 12)
// return pointer to structure scan_zone_result (10 closer objects and their types)
// 0 = no object
// 1 = wall
// 2 = foe robot
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

// robot shoot missile in specific direction
// parameters:
// 	- angle for fire angle in radian (float)
// 	- nothing return
void missile_shoot(float teta){
	int err;
	char message[BUFSIZE] , server_reply[BUFSIZE];

	sprintf(message,"G;%f;",teta);
	if((err = send(fd,message,strlen(message)+1,0))<0)
		printf("send error\n");
	if((err = recv(fd,server_reply,sizeof(server_reply),0))<0)
		printf("receive error\n");
}

//  tell you about the number of missiles fired without explosion
// no parameters
// return :
// 	- a integer for fired missiles
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
