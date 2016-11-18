// Version 3.4 du 20/10/16


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <math.h>
#include "../Serveur/config.h"
#include "../API_Client/client.h"


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
	scan_zone_res.type_scan[0]=0;
	move(1,teta);

	while(vie!=0)
	{

		scan_zone_res = scan_zone(teta_scan);
		
		if (scan_zone_res.type_scan[0]==2 ) // Robot ennemi detecté
		{
			if (scan_zone_res.dist_scan[0]>=100) // si distance trop loin on avance pour se rapprocher
			{
				teta=teta_scan;
				move(3,teta);
			}
			else // On tir
			{
				teta=teta_scan;
				move(0,teta);				
				nbmissile = missile_status();
				if (nbmissile>=0 )
				{
					missile_shoot(teta_scan);
				}				
			}
			teta_scan = teta_scan -15* (M_PI/180);
		}
		else if (scan_zone_res.type_scan[0]==3 && scan_zone_res.dist_scan[0]<=50) // Missile en approche à distance < 50
		{
			teta=teta_scan;
			move(3,teta -10* (M_PI/180));

		}

		else if (scan_zone_res.type_scan[0]==1) // Mur
		{
			move(1,teta -10* (M_PI/180));
		}
		
		else // Pas d'obstacle
		{
			move(1,teta);
		}

		vie = getlife();
	}




	close_connexion();
	return 0;
}
