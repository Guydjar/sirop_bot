// Version 3.3 du 06/10/16

#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
#include "robot_simu.h"
#include "simu.h"
#include "config.h"
#include "wall.h"
#include "graphique.h"


//Mur desctruction
void wall_destroy(s_mur* tab_mur,int j, float x_impact,float y_impact)
{
    //Autorisation config.h et murs différents des 4 murs extérieurs
    if ((WALL_DESTRUCTION_ALLOWED == 1) && (j > 3))
    {

        float x_new = 0;
        float y_new = 0;
        float x_new2 = 0;
        float y_new2 = 0;
        float x_end_temp =0;
        float y_end_temp = 0;
        float dist_wall_from_impact = 0;
        float teta_mur = 0;

        tab_mur[j].exist = 0;
        teta_mur = atan2(tab_mur[j].Ydebut - tab_mur[j].Yfin,tab_mur[j].Xdebut - tab_mur[j].Xfin);
        //printf("teta_wall = %f\n",teta_mur);

        //Calcul de la distance entre le point d'impact et le debut du mur
        dist_wall_from_impact = sqrt(pow(x_impact - tab_mur[j].Xdebut,2) + pow(y_impact - tab_mur[j].Ydebut,2));
        //printf("wall_dist1 = %f\n",dist_wall_from_impact);
        x_end_temp = tab_mur[j].Xfin;
        y_end_temp = tab_mur[j].Yfin;
        //Test si longueur mur à détruire pour savoir si il faut reconstruire un bout du mur
        if (dist_wall_from_impact > MUR_DESTRUC_DIST)
        {
            x_new = MUR_DESTRUC_DIST * cos(teta_mur) +  x_impact;
            y_new = MUR_DESTRUC_DIST * sin(teta_mur) +  y_impact;
            creer_mur(tab_mur,tab_mur[j].Xdebut,tab_mur[j].Ydebut, x_new,y_new);
            //printf("xdeb = %f, ydeb = %f xnew=%f , ynew = %f\n", tab_mur[j].Xdebut,tab_mur[j].Ydebut,x_new,y_new);
        }

        //meme calcul pour la fin du mur
        dist_wall_from_impact = sqrt(pow(x_impact - x_end_temp,2) + pow(y_impact - y_end_temp,2));
        //printf("wall_dist2 = %f\n",dist_wall_from_impact);
        //Construction de la fin ?
        if (dist_wall_from_impact > MUR_DESTRUC_DIST)
        {
        x_new2 = - MUR_DESTRUC_DIST * cos(teta_mur) +  x_impact;
        y_new2 = - MUR_DESTRUC_DIST * sin(teta_mur) +  y_impact;
        creer_mur(tab_mur,x_end_temp,y_end_temp,x_new2,y_new2);
        //printf("xfin = %f, yfin = %f, xnew2=%f , ynew2 = %f\n", x_end_temp,y_end_temp,x_new2,y_new2);
        }
    }
}


//Gestion pour affichage du point d'impact,
//renseigne un tableau pour les impacts des missiles et lance un compteur pour affichage
void missile_impact(s_impact* tab_impact, float x, float y)
{
    int i = 0;
    for (i = 0; i < NB_MISSILE_ALLOWED * NB_ROBOT_MAX; i++)
    {
        if (tab_impact[i].count_impact <= 0)
        {
            tab_impact[i].x = x;
            tab_impact[i].y = y;
            tab_impact[i].count_impact = 10;
            break;
        }
    }
}

void wall_nearest_point(s_robot* tab_robot, s_mur* tab_mur, int id_robot, int id_mur, s_wall* wall, float teta1, float teta2)
{
    int active_debug =0;
    //Calcul des données du mur par rapport au robot
    wall_data(tab_robot, tab_mur, id_robot, id_mur, wall);

    wall->teta_1 = teta1;
    wall->teta_2 = teta2;
//printf("_____teta_1 = %f\n",wall->teta_1);
//printf("_____teta_2 = %f\n",wall->teta_2);
    //Calcul des points et angles nécessaires au 2 droites du scan
    wall->kp = tan(wall->teta_1);
    wall->kp2 = tan(wall->teta_2);
    wall->xp = (tab_robot[id_robot].y - (wall->kp * tab_robot[id_robot].x + tab_mur[id_mur].b)) / ( tab_mur[id_mur].km - wall->kp);
    wall->yp = tab_mur[id_mur].km * wall->xp + tab_mur[id_mur].b;
    wall->d_RP = sqrt(pow(wall->xp - tab_robot[id_robot].x,2) + pow(wall->yp - tab_robot[id_robot].y,2));
    wall->Y_RP = wall->yp - tab_robot[id_robot].y;
    wall->xp2 = (tab_robot[id_robot].y - (wall->kp2 * tab_robot[id_robot].x + tab_mur[id_mur].b)) / ( tab_mur[id_mur].km - wall->kp2);
    wall->yp2 = tab_mur[id_mur].km * wall->xp2 + tab_mur[id_mur].b;
    wall->d_RP2 = sqrt(pow(wall->xp2 - tab_robot[id_robot].x,2) + pow(wall->yp2 - tab_robot[id_robot].y,2));
    wall->Y_RP2 = wall->yp2 - tab_robot[id_robot].y;

    //adaptation si murs verticaux
    if ( tab_mur[id_mur].hv == 1)	//mur verticaux
    {
        wall-> xp = tab_mur[id_mur].Xdebut;
        wall->bp = tab_robot[id_robot].y - wall->kp * tab_robot[id_robot].x;
        wall-> yp = wall->kp * wall-> xp + wall->bp;
        wall->d_RP = sqrt(pow(wall->xp - tab_robot[id_robot].x,2) + pow(wall->yp - tab_robot[id_robot].y,2));
        wall-> xp2 = tab_mur[id_mur].Xdebut;
        wall->bp2 = tab_robot[id_robot].y - wall->kp2 * tab_robot[id_robot].x;
        wall-> yp2 = wall->kp2 * wall-> xp2 + wall->bp2;
        wall->d_RP2 = sqrt(pow(wall->xp2 - tab_robot[id_robot].x,2) + pow(wall->yp2 - tab_robot[id_robot].y,2));
    }


    //scan non compris dans le mur
    if (!(comprisentre(wall->teta_1,wall->teta_d,wall->teta_f) || comprisentre(wall->teta_2,wall->teta_d,wall->teta_f)))
    {
        //printf("scan non compris dans le mur\n");
        wall->dist_proche = 1000000;
        //printf("inversion teta\n");
        wall->teta_1 -= (2*M_PI);
        wall->teta_2 -= (2*M_PI);
    }

    //scan non compris dans le mur meme en negatif
    if (!(comprisentre(wall->teta_1,wall->teta_d,wall->teta_f) || comprisentre(wall->teta_2,wall->teta_d,wall->teta_f)))
    {
        //printf(" 2 scan non compris dans le mur\n");
    }

    //test point le plus proche si perpendiculaire existe
    else if (comprisentre(wall->teta_i,wall->teta_1,wall->teta_2) && comprisentre(wall->teta_i,wall->teta_d,wall->teta_f))
    {
        //printf("perpendiculaire existe\n");
        wall->dist_proche = wall->d_RI;
        wall->xp = wall->xi;
        wall->yp = wall->yi;
    }

    //mur traverse zone
    else if (comprisentre(wall->teta_1,wall->teta_d,wall->teta_f) && comprisentre(wall->teta_2,wall->teta_d,wall->teta_f))
    {
        //compare les 2 distances des croisements de la zone entre droites et mur
        //printf("mur traverse zone\n");
        if (wall->d_RP < wall-> d_RP2)
        {
            wall->dist_proche = wall->d_RP;
        }
        else
        {
            wall->dist_proche = wall->d_RP2;
            wall->xp = wall->xp2;
            wall->yp = wall->yp2;
        }
    }

    //mur compris dans la zone de scan, test si angle debut et fin compris dans scaner
    else if (comprisentre(wall->teta_d,wall->teta_1,wall->teta_2) && (comprisentre(wall->teta_f,wall->teta_1,wall->teta_2)))
    {
        //printf("1 OK		2 OK\n");
        if (wall->d_RD < wall->d_RF)
        {
            wall->dist_proche = wall->d_RD;
            wall->xp = tab_mur[id_mur].Xdebut;
            wall->yp = tab_mur[id_mur].Ydebut;
        }
        else
        {
            wall->dist_proche = wall->d_RF;
            wall->xp = tab_mur[id_mur].Xfin;
            wall->yp = tab_mur[id_mur].Yfin;
        }
    }

    //Xd uniquement compris dans zone
    else if (comprisentre(wall->teta_d,wall->teta_1,wall->teta_2))
    {
        //Teta_1 dans mur
        if (comprisentre(wall->teta_1,wall->teta_d,wall->teta_f))
        {
            //printf("1 OK	d	2 NOK\n");
            if (wall->d_RP < wall->d_RD)
                wall->dist_proche = wall->d_RP;
            else
            {
                wall->dist_proche = wall->d_RD;
                wall->xp = tab_mur[id_mur].Xdebut;
                wall->yp = tab_mur[id_mur].Ydebut;
            }
        }
        //Teta_2 dans mur
        else
        {
            //printf("1 NOK	d	2 OK\n");
            if (wall->d_RP2 < wall->d_RD)
            {
                wall->dist_proche = wall->d_RP2;
                wall->xp = wall->xp2;
                wall->yp = wall->yp2;
            }
            else
            {
                wall->dist_proche = wall->d_RD;
                wall->xp = tab_mur[id_mur].Xdebut;
                wall->yp = tab_mur[id_mur].Ydebut;
            }
        }
    }

    //Xf uniquement
    else if (comprisentre(wall->teta_f,wall->teta_1,wall->teta_2))
    {
        //Teta_1 dans mur
        if (comprisentre(wall->teta_1,wall->teta_d,wall->teta_f))
        {
            //printf("1 OK	f	2 NOK\n");
            if ( wall->d_RP < wall->d_RF)
                wall->dist_proche = wall->d_RP;
            else
            {
                wall->dist_proche = wall->d_RF;
                wall->xp = tab_mur[id_mur].Xfin;
                wall->yp = tab_mur[id_mur].Yfin;
            }
        }
        //Teta_2 dans mur
        else
        {
            //printf("1 NOK	f	2 OK\n");
            if ( wall->d_RP2 < wall->d_RF)
            {
                wall->dist_proche = wall->d_RP2;
                wall->xp = wall->xp2;
                wall->yp = wall->yp2;
            }
            else
            {
            wall->dist_proche = wall->d_RF;
            wall->xp = tab_mur[id_mur].Xfin;
            wall->yp = tab_mur[id_mur].Yfin;
            }
        }
    }

    if (active_debug == 1 && (id_mur == 5||id_mur == 4||id_mur != 6) && id_robot == 8)
    {
        printf("°ri = %f, °RD = %f, °RF = %f\n", (wall->teta_i) * 180/M_PI, (wall->teta_d) * 180/M_PI, (wall->teta_f) * 180/M_PI);
        printf("teta_1 = %f y = %f \n",wall->teta_1* 180/M_PI,tab_robot[id_robot].y);
        printf("teta _ 2 = %f y = %f \n",wall->teta_2* 180/M_PI,tab_robot[id_robot].y);
        printf("kp = %f , xp = %f, yp = %f,\n",wall->kp,wall->xp,wall->yp);
        printf("kp2 = %f , xp2 = %f, yp2 = %f\n",wall->kp2,wall->xp2,wall->yp2);
        printf("d_RP = %f , Y_RP = %f, teta1 = %f,\n",wall->d_RP,wall->Y_RP,wall->teta_1 * 180/M_PI);
        printf("d_RP2 = %f , Y_RP2 = %f, teta2 = %f,\n",wall->d_RP2,wall->Y_RP2,wall->teta_2  * 180/M_PI);
    }
}

void wall_data(s_robot* tab_robot, s_mur* tab_mur, int id_robot, int id_mur, s_wall* wall)
{
	//Equation de la droite du mur
	wall->xi = (tab_robot[id_robot].y - (tab_mur[id_mur].kr * tab_robot[id_robot].x+ tab_mur[id_mur].b)) / ( tab_mur[id_mur].km - tab_mur[id_mur].kr);
//	wall->yi = tab_mur[id_mur].km * ((tab_robot[id_robot].y - tab_mur[id_mur].kr * tab_robot[id_robot].x) / (tab_mur[id_mur].km - tab_mur[id_mur].kr));
	wall->yi = tab_mur[id_mur].km * wall->xi + tab_mur[id_mur].b;
	wall->d_RI = sqrt(pow(wall->xi - tab_robot[id_robot].x,2) + pow(wall->yi - tab_robot[id_robot].y,2));
	wall->d_RD = sqrt(pow(tab_mur[id_mur].Xdebut - tab_robot[id_robot].x,2) + pow(tab_mur[id_mur].Ydebut - tab_robot[id_robot].y,2));
	wall->d_RF = sqrt(pow(tab_mur[id_mur].Xfin - tab_robot[id_robot].x,2) + pow(tab_mur[id_mur].Yfin - tab_robot[id_robot].y,2));
	wall->Y_RI = wall->yi - tab_robot[id_robot].y;
	wall->Y_RD = tab_mur[id_mur].Ydebut - tab_robot[id_robot].y;
	wall->Y_RF = tab_mur[id_mur].Yfin - tab_robot[id_robot].y;
	wall->teta_i = asin(wall->Y_RI/wall->d_RI);
	wall->teta_d = asin(wall->Y_RD/wall->d_RD);
	wall->teta_f = asin(wall->Y_RF/wall->d_RF);

	//adaptation teta_i et xi yi
	if ( tab_mur[id_mur].hv == 1)	//mur verticaux
	{
		if (tab_robot[id_robot].x > tab_mur[id_mur].Xdebut)
			wall->teta_i = M_PI;
		else
			wall->teta_i = 0;

        wall-> yi = tab_robot[id_robot].y;
        wall-> xi = tab_mur[id_mur].Xdebut;
        wall->d_RI = sqrt(pow(wall->xi - tab_robot[id_robot].x,2) + pow(wall->yi - tab_robot[id_robot].y,2));
        //printf("°ri = %f, °RD = %f, °RF = %f\n", (wall->teta_i) * 180/M_PI, (wall->teta_d) * 180/M_PI, (wall->teta_f) * 180/M_PI);
	}
	if ( tab_mur[id_mur].hv == 2)	//mur horizontaux
	{
		if (tab_robot[id_robot].y > tab_mur[id_mur].Ydebut)
			wall->teta_i = -M_PI/2;
		else
			wall->teta_i = M_PI/2;

        wall-> xi = tab_robot[id_robot].x;
        wall-> yi = tab_mur[id_mur].Ydebut;
        wall->d_RI = sqrt(pow(wall->xi - tab_robot[id_robot].x,2) + pow(wall->yi - tab_robot[id_robot].y,2));
        //printf("°ri = %f, °RD = %f, °RF = %f\n", (wall->teta_i) * 180/M_PI, (wall->teta_d) * 180/M_PI, (wall->teta_f) * 180/M_PI);
	}
	//autre mur
	else if ((tab_robot[id_robot].y > wall->yi) && ( tab_mur[id_mur].km < 0))	//robot au dessus du mur et pente négative
    {
        wall->teta_i = - M_PI - wall->teta_i;
    }
    //printf("xi = %f, yi = %f\n",wall->xi,wall->yi);
//		wall->teta_i = - M_PI - wall->teta_i;

	else if ((tab_robot[id_robot].y < wall->yi) && ( tab_mur[id_mur].km > 0)) //robot en dessous du mur et pente positive
	{
		wall->teta_i =  M_PI - wall->teta_i;
	}

	//Adapatation teta_d
	if (tab_robot[id_robot].x > tab_mur[id_mur].Xdebut)
	{
		if (tab_robot[id_robot].y > wall->yi)
		{
		wall->teta_d = -M_PI - wall->teta_d;
		}
		else
		{
			wall->teta_d = M_PI - wall->teta_d;
		}
	}
	//Adapatation teta_f
	if (tab_robot[id_robot].x > tab_mur[id_mur].Xfin)
	{
		if (tab_robot[id_robot].y > wall->yi)
		{
		wall->teta_f = -M_PI - wall->teta_f;
		}
		else
		{
			wall->teta_f = M_PI - wall->teta_f;
		}
	}
}

/* Création Mur */
int creer_mur(s_mur* tab_mur,float Xdebut, float Ydebut, float Xfin, float Yfin)
{
	//verification que la position des murs soient compris dans le terrain sinon retourne -1
	if ((Xdebut < 0 || Xdebut > LARGEUR_TERRAIN) || (Ydebut < 0 || Ydebut > HAUTEUR_TERRAIN) || (Xfin < 0 || Xfin > LARGEUR_TERRAIN) || (Yfin < 0 || Yfin > HAUTEUR_TERRAIN ))
		return -1;
	else
	{
		int i = 0;
		//permet de completer la tab mur si d'autres murs existent
		while (tab_mur[i].exist !=0 && i < NB_WALL_ALLOWED)
		{
			i++;
		}
		//si plus de NB_WALL_ALLOWED murs, ajout impossible, retourne -2
		if (i >= NB_WALL_ALLOWED)
			return -2;

        // le point du ddbut est toujours le y le plus faible
		if (Ydebut < Yfin)
		{
			tab_mur[i].Xdebut = Xdebut;
			tab_mur[i].Ydebut = Ydebut;
			tab_mur[i].Xfin = Xfin;
			tab_mur[i].Yfin = Yfin;
		}
		else
		{
			tab_mur[i].Xdebut = Xfin;
			tab_mur[i].Ydebut = Yfin;
			tab_mur[i].Xfin = Xdebut;
			tab_mur[i].Yfin = Ydebut;
		}
		tab_mur[i].exist = 1;
		tab_mur[i].km = (tab_mur[i].Yfin - tab_mur[i].Ydebut) / (tab_mur[i].Xfin - tab_mur[i].Xdebut);
		tab_mur[i].kr = -1 / tab_mur[i].km;
		tab_mur[i].b = tab_mur[i].Ydebut - tab_mur[i].km * tab_mur[i].Xdebut;
		tab_mur[i].hv = 0;
        //Verification si mur horizontale ou verticale
		if (Xdebut == Xfin)
			tab_mur[i].hv = 1;  //  mur vertical
		if (Ydebut == Yfin)
			tab_mur[i].hv = 2;  //  mur horizontal
	}
return 1;
}

// a compris entre b et c (ou égale)
int comprisentre(float a, float b, float c)
{
	if (c > b)
	{
		if ((a >= b) && (a <= c))
			return 1;
		else
			return 0;
	}
	else
	{
		if ((a <= b) && (a >= c))
			return 1;
		else
			return 0;
	}
}


//Permet d'adapter un angle entre teta_l et teta_h en modulo 2PI
//      ------ ATTENTION------- si angle demandé < à 2PI boucle infini possible
//Prend l'angle à adapter teta
//Renvoie l'angle modulé
float modulo_teta(float teta, float teta_l, float teta_h)
{
    while (!comprisentre(teta, teta_l, teta_h))
    {
        if (teta > teta_h)
            teta -= (2 * M_PI);
        else
            teta += (2 * M_PI);
    }
    return teta;
}

