// Version 3.2 du 11/01/16

//Fichier de configuration du simulateur commun avec graphique et connexion TCP

#ifndef CONFIG_H
#define CONFIG_H

//------------------configuration simulateur----------------------

//robot
#define NB_ROBOT_MAX 10 	//Nombres de robots pouvant jouer (se connecter ????)
#define RAYON_ROBOT 10.5 //rayon du robot en metres
#define ROBOT_MAX_SPEED 3 //vitesse max du robot en m/s

//missile
#define NB_MISSILE_ALLOWED 50 //nombre de missiles pouvant être tirées par robot
#define MISSILE_SPEED 4//vitesse du missile en metres par seconde
#define MISSILE_LENGH 10 //longueur du missile en mètres (utilsés pour position du missile)

//terrain
#define LARGEUR_TERRAIN 800 //largeur du terrain en metres
#define HAUTEUR_TERRAIN 590 //hauteur du terrain en metres
#define MARGE_ECRAN 10 //decalage du terrain par rapport à l'écran
#define HAUTEUR_LOG 200 //hauteur de la fenetre de log
#define LARGEUR_SCORE 300 //largeur de la fenetre de  score
#define LARGEUR_ECRAN (LARGEUR_TERRAIN+LARGEUR_SCORE+(3*MARGE_ECRAN)) // valeur calculée pour la largeur de l'ércan NON CONFIGURABLE !!!!!!!!!!
#define HAUTEUR_ECRAN (HAUTEUR_TERRAIN + HAUTEUR_LOG + (3 * MARGE_ECRAN)) // valeur calculée pour la hauteur de l'écran

//mur
#define NB_WALL_ALLOWED 1000 //Nombre de murs maximum pouvant etre créés
#define MUR_DESTRUC_DIST RAYON_ROBOT // distance de destruction du mur
#define WALL_DESTRUCTION_ALLOWED 1 //autorisation de la detruction des murs
#define NB_WALL_INIT 9 //NB de mur à l'init (hors murs extérieurs (Terrain)
#define DIST_WALL_INIT -1 //Distance des murs -1 pour aléatoire


//scaner allaround
#define NB_SCAN_FIN 8 // nombre de scan utilisés pour le scaner du robot, ex 8, pour 8 zones de 45°
#define SCAN_WITH_TETA_ROBOT 0 //si 1 les zones scannées dépendent de l'angle du robot , si = 0 alors référence fixe zone1 = 0=>45° ....)
#define SCAN_DIST_MAX 200 //Distance max pour le scan around

//scaner zone
#define NB_OBJECT_TO_SCAN 1 //nombre d'objets scannés dans le scan_fin
#define RANGE_SCAN 10// Ouverture en degré du scan pour le scaner par angle (+/- RANGE_SCAN/2)

//get status
#define GAP_DISTANCE 0.1 //Utilisé pour la fonction Get_status pour savoir si le robot touche un objet à plus ou moins GAP_DISTANCE


#endif //CONFIG_H
