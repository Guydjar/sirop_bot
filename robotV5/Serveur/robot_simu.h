// Version 3.3 du 06/10/16
/* liste des fonctions pour le simulateur*/

#ifndef ROBOT_API_H
#define ROBOT_API_H
#include <stdio.h>
#define BUFSIZE 8096
#define CLEF 12345 // je définis une clef au hasard
#define CLEFSEM 98765 // Clef semaphore

#include "config.h"

 typedef struct {
 float zone_distance[8]; // tableau correspondant au scan light pour les 8 zones
 }scan_allaround_result;


 typedef struct {
     float dist_scan[NB_OBJECT_TO_SCAN]; // les NB_OBJECT_TO_SCAN distances les plus courtes pour le scan selectionnée 1000000 si rien
     int type_scan[NB_OBJECT_TO_SCAN]; //type d'objet scanné sur la zone scannée
     float x;
     float y;
 }scan_zone_result;

//	Je définis une structure de memoire partagee.
typedef struct
{
	char tab_cmd[10][4000];
	int flag_w[10];
	int flag_r[10];
	char name[10][40];
	int connected[10];
	int game_status;
}structure_partagee;


//definition des structures
typedef struct {
 	float x;	//position x en metre du centre du robot
 	float y;	//position y en metre du cnetre du robot
 	float teta;	//angle en radian (entre 0 et 2pi)
	float speed; //vitesse du robot
	int life_point;	//Vie restante en pourcent
	int nb_missile;	//Nb de missiles tirés
	int exist; //0 = pas de robot; 1 = robot présent
	char name[40]; //nom du joueur
	float teta_tir; //angle de la tourelle pour affichage
	float teta_scan;
	int count_scan;
	scan_allaround_result scan_all;
	int count_scanLarge;
	scan_zone_result scan_zone;
	int count_explosion;
 }s_robot;


 typedef struct {
     float x;    //position x en metre en tête du missile
     float y;    //position y en metre en tête du missile
     float teta; //angle relatif du missile pas rapport à l'axe x en radian
     int exist; //0 = pas de missile; 1 = missile présent

/******uniqument pour la simu   choix a faire *********************/
	int id_shooter; //permet de savoir qui tue qui

 }s_missile;

  typedef struct {
     float x;    //position x en metre en tête du missile
     float y;    //position y en metre en tête du missile
     int count_impact;
 }s_impact;


 //definit les points de depart et de fin du mur Xdebut, Ydebut Xfin, Yfin  en float
 //ainsi qu'un entier qui nous renseigne sur la présence du mur (0 = pas de mur; 1 = mur présent)
 typedef struct {
     float Xdebut;
     float Ydebut;
     float Xfin;
     float Yfin;
     int exist;
     float km; 	//pente du mur
	 float kr; 	//pente de la perpendiculaire
	 float b; 	//origine de la droite du mur
	 int hv;	//flag pour gérer les spécificités des murs horizontaux ou verticaux (0 = droite, 1 = vertical, 2 = horizontal)
 }s_mur;

// Initialisation des connexions TCP/IP
// prend en paramètre :
//	- un nom de robot
//	- l'adresse ip du serveur pour la connexion
//	- le port pour la connexion
// renvoie id du joueur, renvoie -1 si problème de connexion, code d'erreur à préciser par la suite*/
int init_connexion(char *nom, char *ip,int port);

// permet de récupérer la position d'un robot:
// pas de parametre
// renvoie une structure s_robot*/
s_robot get_position();

// permet de demander de déplacer un robot
// prend en parametre :
//	- la vitesse du robot (speed) en m/s
//	- la direction du robot en radian
// ne renvoie rien*/
void move(float speed, float teta);

// permet de connaitre l'etat du robot, pour savoir si il touche un objet (mur, autre robot)
// pas de parametre
// renvoie 1 char (8bits) pour connaitre quel zone est en contact (bit0 pour zone 1 ... bit7 pour zone 8*/
char get_status();

//A commenter
void close_connexion();

// permet de connaitre le niveau de vie d'un robot
// pas de parametre
// renvoie un entier qui représente le % de vie du robot (0-100)*/
int get_life();

// permet au robot de scanner grossierement à 360° les obstacles sur 8 zones et renvoie la distance la plus courte pour chaque zone
// - pas de parametre
// - renvoie un pointeur vers tableau de 8 floats (1 par zone) correspondant à la distance la plus courte*/
scan_allaround_result scan_allaround();

//permet au robot de scanner finement les obstacles sur une zone de +/- SCAN_LARGE et renvoie les NB_OBJET les plus proches
// prend en parametre:
// 	- l'angle central à scaner (en radians) comprid entre 0 et 2PI
// renvoie une structure scanfin (NB_OBJECT_TO_SCAN objets les plus proches et leurs types)
// 0 = pas d'objet
// 1 = mur
// 2 = robot ennemi
// 3 = missile
scan_zone_result scan_zone(float teta_scan);

// permet au robot de tirer un missile dans une direction donnée
// prend en parametre:
//	- l'angle de tir en radian (float)
//	- ne renvoie rien*/
void missile_shoot(float teta);

// permet de connaitre le nombre de missiles tirés non explosés
// pas de parametre
// renvoie :
//	- un entier qui correspond aux nombres de missiles tirés*/
int missile_status();


#endif // ROBOT_API_H
