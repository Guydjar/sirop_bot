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
	myrobot = get_position(nbplayer-1);
	//scan_around = scan_allaround();
	
	teta=0;
	teta_scan = 0;
	speed = 3;
	vie =100;
	scan_zone_res.type_scan[0]=0;
	move(0,teta_scan);

	while(vie!=0)
	{
		
		while(scan_zone_res.type_scan[0]!=2)
		{
			teta_scan = teta_scan +5* (M_PI/180);
			scan_zone_res = scan_zone(teta_scan);
		}
		move(speed,teta_scan);
		scan_zone_res.type_scan[0]=0;
		nbmissile = missile_status();

		//if (nbmissile!=0 && scan_zone_res.dist_scan[0]<=200)
		//{
			missile_shoot(teta_scan);
		//}
		
		vie = getlife();
	}




	close_connexion();
	return 0;
}
