// Version 4.4 du 12/01/17 - Ronan

//TODO 
// 		Modifier la phase de Traque pour la rendre compatible avec tous les niveaux

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <math.h>
//#include "robot_api.h"
#include "../Serveur/config.h"
#include "../API_Client/client.h"


#define VERSION 1

int i;
char nom[40];
char ip[15];
int nbplayer,port;

s_robot pos_robot; // position du robot
scan_allaround_result scan_around, scan_around_new; 
scan_zone_result scan_zone_res;
float speed,teta=0,teta_scan=0; 
float dist_min=10000;
int vie=5;
int nbmissile, zone_dist_min, env_move, ennemi_just_detected;
int state=0;

int main (int argc, char **argv)
{
	if( argc < 4  || argc > 4 || !strcmp(argv[1], "-?") )
	{
		printf("client for sirop_bot project\n");
		printf("Use ./client <server ip> <port> <name>\n");
		printf("Version : %d\n", VERSION);
		return 0;
	}


	port = atoi(argv[2]);
	sprintf(nom,"%s",argv[3]);
	sprintf(ip,"%s",argv[1]);

	nbplayer = init_connexion(nom,ip,port);
	//myrobot = get_position(nbplayer-1);
	
	state=0;

	while(vie!=0)
	{
		switch (state){
			case 0:
			{
			//////////////////////////////////////////
			//Phase 1: Découverte de l'environnement//
			//////////////////////////////////////////
		
				if (state==0){
					scan_around = scan_allaround(); //1er scan all around
		
					printf("Début phase 0 scans fins milieu zones \n");

					for (i=0; i<8; i++){
						if (scan_around.zone_distance[i]<dist_min){	// Test pour déterminer la distance mini
							dist_min = scan_around.zone_distance[i];
							zone_dist_min=i;
						}
						//printf ("dist min %f en zone %i \n", dist_min, zone_dist_min);
					}
					// Scan fin dans la zone la plus proche
					scan_zone_res = scan_zone((zone_dist_min*45+(45/2))*(M_PI/180)); //Scan fin du milieu de la zone la plus proche
		
					if (scan_zone_res.type_scan[0]==3 && scan_zone_res.dist_scan[0]<=50){ // Missile en approche à distance < 50
						teta=teta_scan;
						while(scan_zone_res.type_scan[0]==3){
							move(3,teta -90*(M_PI/180)); // quart de tour sauve qui peut
							scan_zone_res = scan_zone(teta_scan);
						}
					}
		
					for (i=0; i<8; i++){
						teta_scan=(i*45+(45/2))*(M_PI/180);
						//printf ("teta_scan = %f \n", teta_scan);
						scan_zone_res = scan_zone(teta_scan); // Scan fin au milieu des 7 autres zones
						if (scan_zone_res.type_scan[0]==3 && scan_zone_res.dist_scan[0]<=50){ // Missile en approche à distance < 50
							printf("missile en approche \n");
							teta=teta_scan;
							printf ("missile detecté move teta = %f \n", teta);
							while(scan_zone_res.type_scan[0]==3){
								move(3,teta -90*(M_PI/180)); // quart de tour sauve qui peut
								scan_zone_res = scan_zone(teta_scan);
							}
						}
						else if (scan_zone_res.type_scan[0]==2){ // Robot ennemi detecté
							printf("ennemi détecté \n");
							if (scan_zone_res.dist_scan[0]>=200){ // si distance trop loin on avance pour se rapprocher et on tire
								printf ("ennemi detecté move teta avant de tirer = %f \n", teta);
								teta=teta_scan;
								while (scan_zone_res.type_scan[0]==2 && scan_zone_res.dist_scan[0]>=200){
									move(3,teta);
									if (nbmissile>=0){ // On tire 3 missiles pour balayer la zone
										missile_shoot(teta_scan);
										missile_shoot(teta_scan-(RANGE_SCAN*M_PI/180));
										missile_shoot(teta_scan+(RANGE_SCAN*M_PI/180));
										ennemi_just_detected=1;
									}
									scan_zone_res = scan_zone(teta_scan);
								}

								move(0,teta); // On s'arrête
							}
							else { // On tire			
								nbmissile = missile_status();
								if (nbmissile>=0){
									missile_shoot(teta_scan);
									missile_shoot(teta_scan-(RANGE_SCAN*M_PI/180));
									missile_shoot(teta_scan+(RANGE_SCAN*M_PI/180));
									ennemi_just_detected=1;
								}	
							}
						}
					}
				}
			state=1; // Fin phase d'init
			}
		case 1:
		{
			printf("Début phase 1 scans fins zones complètes \n");
			move(0,teta);
			scan_around_new = scan_allaround(); //2nd scan all around pour vérifier que l'environnement direct n'a pas bougé

			while (env_move==0){
				for (i=0; i<8; i++){// Test pour savoir si l'environnement à bouger		
					if (scan_around_new.zone_distance[i]== scan_around.zone_distance[i]){
						env_move=0; 
						//printf ("env_move = %i \n", env_move);
					}
					else {
						env_move=1; // l'environnement a bougé
						state=2;
						printf ("env_move = %i break \n", env_move);
						break;
					}
				}
				//printf ("Début phase de 1 - 12 premiers scans fin \n");
				for (i=0; i<18; i++){ // Boucle de 12 Scans fin, 3 sur chaque zone (15°*3=45° de 0° à 180°)
					ennemi_just_detected=0; //reinit
					teta_scan=(i*RANGE_SCAN)*(M_PI/180);
					//printf ("teta_scan = %f \n", teta_scan);
					scan_zone_res = scan_zone(teta_scan); 
				
					// actions en fct du résultat du scan
					if (scan_zone_res.type_scan[0]==3 && scan_zone_res.dist_scan[0]<=50){ // Missile en approche à distance < 50
						printf("missile en approche \n");
						teta=teta_scan;
						printf ("missile detecté shoot teta = %f \n", teta);
						if(ennemi_just_detected==0){ //Signifie qu'on vient pas de tirer, évite de détecter son missile
							while(scan_zone_res.type_scan[0]==3){
								move(3,teta -90*(M_PI/180)); // quart de tour sauve qui peut
								scan_zone_res = scan_zone(teta_scan);
							}
						}
					}
					else if (scan_zone_res.type_scan[0]==2){ // Robot ennemi detecté
						printf("ennemi détecté \n");
						if (scan_zone_res.dist_scan[0]>=200){ // si distance trop loin on avance pour se rapprocher et on tire
							printf ("ennemi detecté move teta pour rapprocher avant de tirer = %f \n", teta);
							teta=teta_scan;
							while (scan_zone_res.type_scan[0]==2 && scan_zone_res.dist_scan[0]>=200){
								move(3,teta);
								if (nbmissile>=0){
									missile_shoot(teta_scan);
									missile_shoot(teta_scan);
									missile_shoot(teta_scan);
									//printf ("Je tire de teta = %f \n", teta);
								}
								scan_zone_res = scan_zone(teta_scan);
							}
							// on sort de cette boucle car on bouge
							env_move=1;
							ennemi_just_detected=1;
							state=2;
						}
						else {	// On tire
							move(0,teta);				
							nbmissile = missile_status();
							while (scan_zone_res.type_scan[0]==2) {
								if (nbmissile>=0){
									missile_shoot(teta_scan);
									missile_shoot(teta_scan);
									missile_shoot(teta_scan);
									//printf ("Je tire de teta = %f \n", teta);
								}	
								scan_zone_res = scan_zone(teta_scan);
							}		
						}
					}
				}

				scan_around_new = scan_allaround(); // Après avoir analysé 4 zones finement on refait un scan all around 
				for (i=0; i<8; i++){// Test pour savoir si l'environnement à bouger		
					if (scan_around_new.zone_distance[i]== scan_around.zone_distance[i]){
						env_move=0; 
					}
					else {
						env_move=1; // l'environnement a bougé
						printf ("env_move = %i break \n", env_move);
						state=2;
						break;
					}
				}
				//printf ("phase de 1 - 12 derniers scans fin \n");
				for (i=18; i<36; i++){ // Boucle de 12 Scans fin pour finir le tour 180° à 360°)
					ennemi_just_detected=0; //reinit
					teta_scan=(i*RANGE_SCAN)*(M_PI/180);
					//printf ("teta_scan = %f \n", teta_scan);
					scan_zone_res = scan_zone(teta_scan);
				
					// actions en fct du résultat du scan
					if (scan_zone_res.type_scan[0]==3 && scan_zone_res.dist_scan[0]<=50){ // Missile en approche à distance < 50
						printf("missile en approche \n");
						teta=teta_scan;
						printf ("missile detecté move teta = %f \n", teta);
						if(ennemi_just_detected==0){ //Signifie qu'on vient pas de tirer, évite de détecter son missile
							while(scan_zone_res.type_scan[0]==3){
								move(3,teta -90*(M_PI/180)); // quart de tour sauve qui peut
								scan_zone_res = scan_zone(teta_scan);
							}
						}
					}
					else if (scan_zone_res.type_scan[0]==2){ // Robot ennemi detecté
						printf("ennemi détecté \n");
						if (scan_zone_res.dist_scan[0]>=200){ // si distance trop loin on avance pour se rapprocher
							printf ("ennemi detecté move teta pour se rapprocher avant de tirer = %f \n", teta);
							teta=teta_scan;
							while (scan_zone_res.type_scan[0]==2 && scan_zone_res.dist_scan[0]>=200){
								move(3,teta);
								if (nbmissile>=0){
									missile_shoot(teta_scan);
									missile_shoot(teta_scan);
									missile_shoot(teta_scan);
								}
								scan_zone_res = scan_zone(teta_scan);
							}
							env_move=1;
							ennemi_just_detected=1;
							state=2;
						}
						else { // On tir
							teta=teta_scan;
							move(0,teta);				
							nbmissile = missile_status();
							while (scan_zone_res.type_scan[0]==2){
								if (nbmissile>=0){
									missile_shoot(teta_scan);
									missile_shoot(teta_scan);
									missile_shoot(teta_scan);
									//printf ("Je tire de teta = %f \n", teta);
								}	
								scan_zone_res = scan_zone(teta_scan);
							}
						}
					}
				}
				printf("fin phase 1 - scans fins zones complètes \n");
				env_move=2; //permet de sortir de la boucle et de passer en phase 3, où on va bouger 
			}

			scan_around_new = scan_allaround(); // Après avoir analysé 4 zones finement on refait un scan all around 
			state=3;// On passe à la phase 3, pour traquer l'ennemi

		}

		///////////////////////////////////////////////////////////////////////
		//Phase 2: Environnement vient de bouger on essaye de suivre l'ennemi//
		///////////////////////////////////////////////////////////////////////
		case 2:
		{
			while (env_move==1){
				printf("Début phase 2 - Environnement a bougé, on traque l'ennemi \n");
				move(0,teta);

				for (i=0; i<36; i++){ // Boucle de 36 Scans fin, 3 sur chaque zone (15°*3=45° de 0° à 180°)
					ennemi_just_detected=0; //reinit
					teta_scan=teta_scan+(i*RANGE_SCAN)*(M_PI/180); // On repart du dernier teta_scan car l'ennemi est surement proche
					scan_zone_res = scan_zone(teta_scan); 
					if (scan_zone_res.type_scan[0]==3 && scan_zone_res.dist_scan[0]<=50){ // Missile en approche à distance < 50
						printf("missile en approche \n");
						teta=teta_scan;
						printf ("missile detecté move teta = %f \n", teta);
						if(ennemi_just_detected==0){ //Signifie qu'on vient pas de tirer, évite de détecter son missile
							while(scan_zone_res.type_scan[0]==3){
								move(3,teta -90*(M_PI/180)); // quart de tour sauve qui peut
								scan_zone_res = scan_zone(teta_scan);
							}
						}
					}
					else if (scan_zone_res.type_scan[0]==2){ // Robot ennemi detecté
						printf("ennemi détecté \n");
						if (scan_zone_res.dist_scan[0]>=200){ // si distance trop loin on avance pour se rapprocher
							printf ("ennemi detecté move teta pour se rapprocher avant de tirer = %f \n", teta);
							teta=teta_scan;
							while (scan_zone_res.type_scan[0]==2 && scan_zone_res.dist_scan[0]>=200){
								move(3,teta);
								if (nbmissile>=0){
									missile_shoot(teta_scan);
									missile_shoot(teta_scan);
									missile_shoot(teta_scan);
								}
								scan_zone_res = scan_zone(teta_scan);
							}
							//move(0,teta);
						}
						else { // On tir
							teta=teta_scan;
							move(0,teta);				
							nbmissile = missile_status();
							while (scan_zone_res.type_scan[0]==2) {
								if (nbmissile>=0){
									missile_shoot(teta_scan);
									missile_shoot(teta_scan);
									missile_shoot(teta_scan);
									//printf ("Je tire de teta = %f \n", teta);
								}	
								scan_zone_res = scan_zone(teta_scan);
							}
						}
					} //TODO a modifier
					/*else if (scan_zone_res.type_scan[0]==1 && scan_zone_res.dist_scan[0]<=10){ // Mur detecté
						printf ("Pas de missle, ni ennemi detecté et Mur très proche on bouge de 90°C \n");
						teta=teta_scan+(90)*(M_PI/180);
						move(3,teta);
					}*/
				}
				state=1; // On analyse l'environnement
				env_move=0;
				printf("Fin phase 2, on passe en phase 1 scans fins zones complètes \n");
			}
		}

		////////////////////////////////////////////////////////////
		//Phase 3: Déplacement à la recherche d'ennemi - MAP niv 1//
		////////////////////////////////////////////////////////////
		case 3:
		{
			while (env_move==2){
				printf("Début phase 3 - Environnement n'a pas bougé, on traque l'ennemi \n");
				move(0,teta);
				pos_robot=get_position(nbplayer-1);
				printf("x = %f y = %f \n", pos_robot.x, pos_robot.y);
				if (pos_robot.y > HAUTEUR_TERRAIN/2){ // le robot se trouve dans la seconde moitié haute			
					if (pos_robot.x > LARGEUR_TERRAIN/2){ // le robot se trouve dans la partie droite de la MAP
						if (pos_robot.y > 2*HAUTEUR_TERRAIN/3) {
							while (pos_robot.x > LARGEUR_TERRAIN/2-20){
								teta=M_PI; // déplacement vers la gauche			
								move(3,teta);
								//printf("Déplacement vers la gauche, teta = %f \n", teta);
								pos_robot=get_position(nbplayer-1);
							}
							while (pos_robot.y > HAUTEUR_TERRAIN/2+20){
								teta=-M_PI/2; // déplacement vers le bas		
								move(3,teta);
								//printf("Deplacement vers le bas, teta = %f \n", teta);
								pos_robot=get_position(nbplayer-1);
							}
							while (pos_robot.x > LARGEUR_TERRAIN/2+20){
								teta=0; // déplacement vers la droite			
								move(3,teta);
								//printf("Déplacement vers la droite, teta = %f \n", teta);
								pos_robot=get_position(nbplayer-1);
							}
							state=1; // On analyse l'environnement
							env_move=0;
							printf("Fin phase 3, on passe en phase 1 scans fins zones complètes \n");
						}	
						else{
							while (pos_robot.y > HAUTEUR_TERRAIN/2-20){
								teta=-M_PI/2; // déplacement vers le bas		
								move(3,teta);
								//printf("Deplacement vers le bas, teta = %f \n", teta);
								pos_robot=get_position(nbplayer-1);
							}
							while (pos_robot.x > LARGEUR_TERRAIN/2-20){
								teta=0; // déplacement vers la gauche			
								move(3,M_PI);
								//printf("Déplacement vers la gauche, teta = %f \n", teta);
								pos_robot=get_position(nbplayer-1);
							}
							while (pos_robot.y > HAUTEUR_TERRAIN/3-60){
								teta=-M_PI/2; // déplacement vers le bas		
								move(3,teta);
								//printf("Deplacement vers le bas, teta = %f \n", teta);
								pos_robot=get_position(nbplayer-1);
							}
							state=1; // On analyse l'environnement
							env_move=0;
							printf("Fin phase 3, on passe en phase 1 scans fins zones complètes \n");
						}
					}
					else { // robot est dans la partie gauche
						while (pos_robot.y > HAUTEUR_TERRAIN/2+20){
							teta=-M_PI/2; // déplacement vers le bas		
							move(3,teta);
							//printf("Deplacement vers le bas, teta = %f \n", teta);
							pos_robot=get_position(nbplayer-1);
						}
						while (pos_robot.x < LARGEUR_TERRAIN/2+20){
							teta=0; // déplacement vers la droite			
							move(3,teta);
							//printf("Déplacement vers la droite, teta = %f \n", teta);
							pos_robot=get_position(nbplayer-1);
						}
						state=1; // On analyse l'environnement
						env_move=0;
						printf("Fin phase 3, on passe en phase 1 scans fins zones complètes \n");
					}
				}
				else { // le robot se trouve dans la seconde moitié basse				
					if (pos_robot.x > LARGEUR_TERRAIN/2){ // le robot se trouve dans la partie droite de la MAP	
						if (pos_robot.y < HAUTEUR_TERRAIN/3) {
							while (pos_robot.x > LARGEUR_TERRAIN/2-20){
								teta=M_PI; // déplacement vers la gauche			
								move(3,teta);
								//printf("Déplacement vers la gauche, teta = %f \n", teta);
								pos_robot=get_position(nbplayer-1);
							}
							while (pos_robot.y < HAUTEUR_TERRAIN/2-20){
								teta=M_PI/2; // déplacement vers le haut		
								move(3,teta);
								//printf("Deplacement vers le haut, teta = %f \n", teta);
								pos_robot=get_position(nbplayer-1);
							}
							while (pos_robot.x < LARGEUR_TERRAIN/2+20){
								teta=0; // déplacement vers la droite			
								move(3,teta);
								//printf("Déplacement vers la droite, teta = %f \n", teta);
								pos_robot=get_position(nbplayer-1);
							}
							state=1; // On analyse l'environnement
							env_move=0;
							printf("Fin phase 3, on passe en phase 1 scans fins zones complètes \n");
						}
						else {
							while (pos_robot.y < HAUTEUR_TERRAIN/2+20){
								teta=M_PI/2; // déplacement vers le haut		
								move(3,teta);
								//printf("Deplacement vers le haut, teta = %f \n", teta);
								pos_robot=get_position(nbplayer-1);
							}
							while (pos_robot.x > LARGEUR_TERRAIN/2-20){
								teta=0; // déplacement vers la gauche			
								move(3,M_PI);
								//printf("Déplacement vers la gauche, teta = %f \n", teta);
								pos_robot=get_position(nbplayer-1);
							}
							while (pos_robot.y < 2*HAUTEUR_TERRAIN/3+60){
								teta=M_PI/2; // déplacement vers le haut		
								move(3,teta);
								//printf("Deplacement vers le haut, teta = %f \n", teta);
								pos_robot=get_position(nbplayer-1);
							}
							state=1; // On analyse l'environnement
							env_move=0;
							printf("Fin phase 3, on passe en phase 1 scans fins zones complètes \n");
						}	
					}
					else { // le robot se trouve dans la partie gauche de la MAP
						while (pos_robot.y < HAUTEUR_TERRAIN/2-20){
							teta=M_PI/2; // déplacement vers le haut			
							move(3,teta);
							//printf("Deplacement vers le haut, teta = %f \n", teta);
							pos_robot=get_position(nbplayer-1);
						}
						while (pos_robot.x < LARGEUR_TERRAIN/2+20){
							teta=0; // déplacement vers la droite			
							move(3,teta);
							//printf("Déplacement vers la droite, teta = %f \n", teta);
							pos_robot=get_position(nbplayer-1);
						}
						state=1; // On analyse l'environnement
						env_move=0;
						printf("Fin phase 3, on passe en phase 1 scans fins zones complètes \n");
					}
				}
			}
			
		}
		vie = get_life();
		}
	
	}

	close_connexion();
	return 0;
}
