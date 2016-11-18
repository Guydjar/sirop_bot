// Version 3.5 du 03/11/16

//Contient tous les prototypes et les structures des fonctions pour les clients


#ifndef CLIENT_H_INCLUDED
#define CLIENT_H_INCLUDED

#define BUFSIZE 8096
#define CLEF 12345 // je définis une clef au hasard
#define CLEFSEM 98765 // Clef semaphore

//////////////////////////////////////////////////////////////
// Definition des structures
//////////////////////////////////////////////////////////////

typedef struct {
 	float x;	//position x en metre du centre du robot
 	float y;	//position y en metre du centre du robot
 	float teta;	//angle en radian (entre 0 et 2pi)
	int exist; //0 = pas de robot; 1 = robot présent
 }s_robot;

 typedef struct {
     float x;    //position x en metre en tête du missile
     float y;    //position y en metre en tête du missile
     float teta; //angle relatif du missile pas rapport à l'axe x en radian
     int exist; //0 = pas de missile; 1 = missile présent
 }s_missile;

// Définition de la structure pour le scan allaround, un tableau de 8 floats correspondant aux 8 zones
 typedef struct {
 float zone_distance[8]; 
 }scan_allaround_result;

// Définition de la structure pour le scan fin
 typedef struct {
     float dist_scan[NB_OBJECT_TO_SCAN]; // les NB_OBJECT_TO_SCAN distances les plus courtes pour le scan selectionnée 1000000 si rien
     int type_scan[NB_OBJECT_TO_SCAN]; //type d'objet scanné sur la zone scannée
     float x; // distance suivant axe des x
     float y;  // distance suivant axe des y
 }scan_zone_result;


//////////////////////////////////////////////////////////////
// Définition des Fonctions 
//////////////////////////////////////////////////////////////

// Initialisation des connexions TCP/IP
// prend en paramètre :
//	- un nom de robot
//	- l'adresse ip du serveur pour la connexion
//	- le port pour la connexion
// renvoie id du joueur, renvoie -1 si problème de connexion, code d'erreur à préciser par la suite*/
int init_connexion(char *nom, char *ip,int port);

//A commenter
void close_connexion();

// Fonction permet de récupérer la position d'un robot:
// pas de parametre
// renvoie une structure s_robot*/
s_robot get_position();

// Fonction permet de demander de déplacer un robot
// prend en parametre :
//	- la vitesse du robot (speed) en m/s
//	- la direction du robot en radian
// ne renvoie rien*/
void move(float speed, float teta);

// Fonction permet de connaitre l'etat du robot, pour savoir si il touche un objet (mur, autre robot)
// Ne consomme pas de parametre
// renvoie 1 char (8bits) pour connaitre quelle zone est en contact (bit0 pour zone 1 ... bit7 pour zone 8*/) bit X = 1 si ça touche et =0 si pas touche
// La définition des zones est définie dans simu.c fct scan_allaround_result
// 2 possibilités cf. SCAN_WITH_TETA_ROBOT: 
		//Si = 0 => Zone 0 est fonction de l'orientation du déplacement du robot (Teta_robot)
		//Si = 1 => Zone 0 est fixe, à partir de l'angle entre 0 et 2Pi/8 radians (soit Est / Nord-Est)

// NB: Get_status n'apporte pas grand chose, scan_allaround est plus complet
char get_status();

// Fonction permet de connaitre le niveau de vie d'un robot
// pas de parametre
// renvoie un entier qui représente le % de vie du robot (0-100)*/
int get_life();


// fonction qui permet au robot de scanner grossierement à 360° les obstacles sur 8 zones et renvoie la distance la plus courte pour chaque zone
// - pas de parametre
// - renvoie un pointeur vers tableau de 8 floats (1 par zone) correspondant à la distance la plus courte de chaque zone
// La définition des zones est définie dans simu.c fct scan_allaround_result
// 2 possibilités cf. SCAN_WITH_TETA_ROBOT: 
		//Si = 0 => Zone 0 est fonction de l'orientation du déplacement du robot (Teta_robot)
		//Si = 1 => Zone 0 est fixe, à partir de l'angle entre 0 et 2Pi/8 radians (soit Est / Nord-Est)

//TODO: Rajouter un lien doxygene vers un schéma avec les distances de chaques zones scannées 

scan_allaround_result scan_allaround();


// Fonction qui permet au robot de scanner finement les obstacles sur une zone de +/- SCAN_LARGE et renvoie les NB_OBJET les plus proches
// prend en parametre:
// 	- l'angle central à scaner (en radians) comprid entre 0 et 2PI
// renvoie une structure scanfin (NB_OBJECT_TO_SCAN objets les plus proches et leurs types)
// 0 = pas d'objet
// 1 = mur
// 2 = robot ennemi
// 3 = missile
scan_zone_result scan_zone(float teta_scan);

// Fonction permet au robot de tirer un missile dans une direction donnée
// prend en parametre:
//	- l'angle de tir en radian (float)
//	- ne renvoie rien*/
void missile_shoot(float teta);

// Fonction permet de connaitre le nombre de missiles tirés non explosés
// pas de parametre
// renvoie :
//	- un entier qui correspond aux nombres de missiles tirés*/
int missile_status();


#endif // CLIENT_H_INCLUDED
