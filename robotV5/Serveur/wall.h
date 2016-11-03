// Version 3.2 du 11/01/16



#ifndef WALL_H
#define WALL_H

#include "graphique.h"


// structure permettant de gérer les paramètres entre un robot et un mur (angla debut, perpendiculaire et fin du mur / au robot) et distance
 typedef struct {
     float xi;
     float yi;
     float d_RI;
     float d_RD;
     float d_RF;
     float Y_RI;
     float Y_RD;
     float Y_RF;
     float teta_i;
     float teta_d;
     float teta_f;
	float teta_p;
	float d_RP;
    float Y_RP;
	float kp;
	float xp;
	float yp;
	float bp;
	float d_RP2;
    float Y_RP2;
	float kp2;
	float xp2;
	float yp2;
	float bp2;
	float teta_1;
	float teta_2;
	float dist_proche;

 }s_wall;

int creer_mur(s_mur* tab_mur,float Xdebut, float Ydebut, float Xfin, float Yfin);

void wall_data(s_robot* tab_robot, s_mur* tab_mur, int id_robot, int id_mur, s_wall* wall);

int comprisentre(float a, float b, float c);

void wall_nearest_point(s_robot* tab_robot, s_mur* tab_mur, int id_robot, int id_mur, s_wall* wall, float teta1, float teta2);

float modulo_teta(float teta, float teta_l, float teta_h);

void missile_impact(s_impact* tab_impact, float x, float y);

void wall_destroy(s_mur* tab_mur,int j, float x_impact,float y_impact);



#endif //WALL_H
