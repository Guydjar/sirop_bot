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


#define VERSION 1







int main (int argc, char **argv)
{
	if( argc < 4  || argc > 4 || !strcmp(argv[1], "-?") )
	{
		printf("client for sirop_bot project\n");
		printf("Use ./client <server ip> <port> <name>\n");
		printf("Version : %d\n", VERSION);
		return 0;
	}

	char nom[40];
	char ip[15];
	int nbplayer,port;

	// Variable de conversion
	s_robot myrobot;
	scan_allaround_result scan_around;
	scan_zone_result scan_zone_res;
	float speed,teta,teta_scan;
	int vie,  nbmissile;
	//int zone;
	//char c;

	port = atoi(argv[2]);
	sprintf(nom,"%s",argv[3]);
	sprintf(ip,"%s",argv[1]);

	nbplayer = init_connexion(nom,ip,port);

	/*myrobot = get_position(nbplayer);
	printf("%f;%f;%f;%d;\n",myrobot.x,myrobot.y,myrobot.teta,myrobot.exist);

	move(speed,teta);

	c = get_status(nbplayer);
	printf("%c\n",c);

	vie = getlife();
	printf("%d\n",vie);

	scanli = scan_light();
	printf("%f;%f;%f;%f;%f;%f;%f;%f;\n",scanli.zone_distance[0],scanli.zone_distance[1],scanli.zone_distance[2],scanli.zone_distance[3],scanli.zone_distance[4],scanli.zone_distance[5],scanli.zone_distance[6],scanli.zone_distance[7]);

	scanfi = scan_fin(8);
	printf("%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;\n",scanfi.dist_scan[0],scanfi.dist_scan[1],scanfi.dist_scan[2],scanfi.dist_scan[3],scanfi.dist_scan[4],scanfi.dist_scan[5],scanfi.dist_scan[6],scanfi.dist_scan[7],scanfi.dist_scan[8],scanfi.dist_scan[9],scanfi.type_scan[0],scanfi.type_scan[1],scanfi.type_scan[2],scanfi.type_scan[3],scanfi.type_scan[4],scanfi.type_scan[5],scanfi.type_scan[6],scanfi.type_scan[7],scanfi.type_scan[8],scanfi.type_scan[9]);

	missile_shoot(teta);

	nbmissile = missile_status();
	printf("%d\n",vie);

	myrobot = get_position(nbplayer);
	printf("%f;%f;%f;%d;",myrobot.x,myrobot.y,myrobot.teta,myrobot.exist);

	move(speed,teta);

	c = get_status(nbplayer);
	printf("%c\n",c);

	vie = getlife();
	printf("%d\n",vie);

	scan_around = scan_light();
	printf("%f;%f;%f;%f;%f;%f;%f;%f;\n",scanli.zone_distance[0],scanli.zone_distance[1],scanli.zone_distance[2],scanli.zone_distance[3],scanli.zone_distance[4],scanli.zone_distance[5],scanli.zone_distance[6],scanli.zone_distance[7]);

	scanfi = scan_fin(8);
	printf("%f;%f;%f;%f;%f;%f;%f;%f;%f;%f;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;\n",scanfi.dist_scan[0],scanfi.dist_scan[1],scanfi.dist_scan[2],scanfi.dist_scan[3],scanfi.dist_scan[4],scanfi.dist_scan[5],scanfi.dist_scan[6],scanfi.dist_scan[7],scanfi.dist_scan[8],scanfi.dist_scan[9],scanfi.type_scan[0],scanfi.type_scan[1],scanfi.type_scan[2],scanfi.type_scan[3],scanfi.type_scan[4],scanfi.type_scan[5],scanfi.type_scan[6],scanfi.type_scan[7],scanfi.type_scan[8],scanfi.type_scan[9]);

	missile_shoot(teta);

	nbmissile = missile_status();
	printf("%d\n",vie);*/
	speed = 3;
	teta = M_PI/2;
	move(speed,teta);

	teta = M_PI/4;
	missile_shoot(teta);

	myrobot = get_position(nbplayer-1);
	printf("x : %f;y : %f;teta : %f;exist : %d;\n",myrobot.x,myrobot.y,myrobot.teta,myrobot.exist);

	vie = getlife();
	printf("vie : %d\n",vie);

	nbmissile = missile_status();
	printf("%d\n",nbmissile);

	scan_around = scan_allaround();
	printf("%f;%f;%f;%f;%f;%f;%f;%f;\n",scan_around.zone_distance[0],scan_around.zone_distance[1],scan_around.zone_distance[2],scan_around.zone_distance[3],scan_around.zone_distance[4],scan_around.zone_distance[5],scan_around.zone_distance[6],scan_around.zone_distance[7]);

	teta_scan = M_PI/4;
	scan_zone_res = scan_zone(teta_scan);
	printf("%f;%d;\n",scan_zone_res.dist_scan[0],scan_zone_res.type_scan[0]);

	sleep(3);

	speed = 3;
	teta = 2 *M_PI;
	move(speed,teta);
	teta = M_PI;
	missile_shoot(teta);
	myrobot = get_position(nbplayer-1);
	printf("x : %f;y : %f;teta : %f;exist : %d;\n",myrobot.x,myrobot.y,myrobot.teta,myrobot.exist);
	vie = getlife();
	printf("vie : %d\n",vie);
	nbmissile = missile_status();
	printf("%d\n",nbmissile);
		scan_around = scan_allaround();
	printf("%f;%f;%f;%f;%f;%f;%f;%f;\n",scan_around.zone_distance[0],scan_around.zone_distance[1],scan_around.zone_distance[2],scan_around.zone_distance[3],scan_around.zone_distance[4],scan_around.zone_distance[5],scan_around.zone_distance[6],scan_around.zone_distance[7]);

	teta_scan = M_PI/4;
	scan_zone_res = scan_zone(teta_scan);
	printf("%f;%d;\n",scan_zone_res.dist_scan[0],scan_zone_res.type_scan[0]);
	sleep(3);

	speed = 3;
	teta = M_PI;
	move(speed,teta);
	teta = M_PI/4;
	missile_shoot(teta);
	myrobot = get_position(nbplayer-1);
	printf("x : %f;y : %f;teta : %f;exist : %d;\n",myrobot.x,myrobot.y,myrobot.teta,myrobot.exist);
	vie = getlife();
	printf("vie : %d\n",vie);
	nbmissile = missile_status();
	printf("%d\n",nbmissile);
		scan_around = scan_allaround();
	printf("%f;%f;%f;%f;%f;%f;%f;%f;\n",scan_around.zone_distance[0],scan_around.zone_distance[1],scan_around.zone_distance[2],scan_around.zone_distance[3],scan_around.zone_distance[4],scan_around.zone_distance[5],scan_around.zone_distance[6],scan_around.zone_distance[7]);

	teta_scan = M_PI/4;
	scan_zone_res = scan_zone(teta_scan);
	printf("%f;%d;\n",scan_zone_res.dist_scan[0],scan_zone_res.type_scan[0]);
	sleep(3);

	speed = 3;
	teta = 3*M_PI/4;
	move(speed,teta);
	teta = M_PI/4;
	missile_shoot(teta);
	myrobot = get_position(nbplayer-1);
	printf("x : %f;y : %f;teta : %f;exist : %d;\n",myrobot.x,myrobot.y,myrobot.teta,myrobot.exist);
	vie = getlife();
	printf("vie : %d\n",vie);
	nbmissile = missile_status();
	printf("%d\n",nbmissile);
		scan_around = scan_allaround();
	printf("%f;%f;%f;%f;%f;%f;%f;%f;\n",scan_around.zone_distance[0],scan_around.zone_distance[1],scan_around.zone_distance[2],scan_around.zone_distance[3],scan_around.zone_distance[4],scan_around.zone_distance[5],scan_around.zone_distance[6],scan_around.zone_distance[7]);

	teta_scan = M_PI/4;
	scan_zone_res = scan_zone(teta_scan);
	printf("%f;%d;\n",scan_zone_res.dist_scan[0],scan_zone_res.type_scan[0]);
	sleep(3);



	close_connexion();
	return 0;
}
