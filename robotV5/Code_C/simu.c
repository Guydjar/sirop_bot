// Version 3.2 du 11/01/16

#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
#include "robot_api.h"
#include "wall.h"
#include "simu.h"
#include "config.h"
#include "graphique.h"

Uint8 simu_get_status(s_robot* tab_robot,s_mur* tab_mur,s_missile* tab_missile, int id)
{
    int i = 0;
    Uint8 result = 0;
    scan_allaround_result scan;
    scan = simu_scan_allaround(tab_robot, tab_mur, tab_missile, id);
    printf("status\n");
    //Test des 8 zones du scanaround si points sont collés au robot ou non
    for (i = 0 ; i < 8 ; i++)
    {
        if (scan.zone_distance[i] < (RAYON_ROBOT + GAP_DISTANCE))
        {
            //printf("res_init = %d\n",result);
            result = result | (1 << i);
            //printf("res = %d\n",result);
        }
    }
    return result;
}


//Scan les zones
scan_allaround_result simu_scan_allaround(s_robot* tab_robot,s_mur* tab_mur,s_missile* tab_missile, int id)
{
	int active_debug =0;
	s_wall wall;
	int i = 0;
	int zone_nb = 0;
	scan_allaround_result scan;
	float teta_1 = 0;
	float teta_2 = 0;
	float dist_robot = 0;
    float teta_robot=0;
    float dist_missile = 0;
    float teta_missile=0;

    //Calcul des points d'impacts murs robots et missile
	for (zone_nb = 1 ; zone_nb <= 8 ; zone_nb++)
	{
		scan.zone_distance[zone_nb-1] = 1000000;

		//Calcul des teta_1 et 2 en fonction du repère selectionné (config.h) angle robot pris en compte ou non
        if (SCAN_WITH_TETA_ROBOT)
        {
            teta_1 = tab_robot[id].teta + ((zone_nb -1) * 2*M_PI / NB_SCAN_FIN);
            teta_2 = tab_robot[id].teta + (zone_nb * 2*M_PI / NB_SCAN_FIN);
        }
        else
        {
            teta_1 = ((zone_nb -1) * 2*M_PI / NB_SCAN_FIN);
            teta_2 = (zone_nb * 2*M_PI / NB_SCAN_FIN);
        }

        //Calcul des points par rapport aux murs
		for (i = 0; i <  NB_WALL_ALLOWED; i++) //NB_WALL_ALLOWED
		{
			if (tab_mur[i].exist == 1)
			{
				wall_nearest_point(tab_robot, tab_mur, id, i, &wall,teta_1,teta_2);

				//TEst si valeur est < à l'actuelle et compris entre 0 et distance max detectable (config.h)
				if ((scan.zone_distance[zone_nb-1] > wall.dist_proche) && (wall.dist_proche >= 0) && (wall.dist_proche <= SCAN_DIST_MAX))
					scan.zone_distance[zone_nb-1] = wall.dist_proche;
			}
		}

		//scan des robots
        for (i = 0; i < NB_ROBOT_MAX; i++)
        {
            //test robot présent et différent de celui qui scanne
            if ((i != id) && (tab_robot[i].exist ==1)) //scan uniquement si différent du robot qui scanne et existant
            {
                //Calcul des la distance et de l'angle qui les séparent en leur centre
                dist_robot = sqrt(pow(tab_robot[i].x - tab_robot[id].x,2) + pow(tab_robot[i].y - tab_robot[id].y,2)) - RAYON_ROBOT;
                teta_robot = atan2((tab_robot[i].y - tab_robot[id].y), (tab_robot[i].x - tab_robot[id].x));
                //Adaptation teta_robot entre 0 et 2PI
                teta_robot = modulo_teta(teta_robot,0,2 * M_PI);
                //printf("dist_robot%d = %f et teta = %f\n",i,dist_robot,teta_robot);

                // Robot dans la zone de scan
                if (comprisentre(teta_robot,teta_1,teta_2))
                {
                    // test inférieur a valeur précédente et < à distance max de detection
                    if ((scan.zone_distance[zone_nb-1] > dist_robot) && (dist_robot <= SCAN_DIST_MAX))
                    {
                        scan.zone_distance[zone_nb-1] = dist_robot;

                        if (active_debug)
                        {
                            printf("dist_scannée = %f robot %d\n", scan.zone_distance[zone_nb-1],i);
                        }
                    }
                }
            }
        }

        //scan des missiles
        for (i = 0; i < NB_ROBOT_MAX * NB_MISSILE_ALLOWED; i++)
        {
            if ((tab_missile[i].exist == 1)) //scan uniquement si missile existe
            {
                //Calcul distance et angle
                dist_missile = sqrt(pow(tab_missile[i].x - tab_robot[id].x,2) + pow(tab_missile[i].y - tab_robot[id].y,2));
                teta_missile = atan2((tab_missile[i].y - tab_robot[id].y), (tab_missile[i].x - tab_robot[id].x));
                //Adaptation missile entre 0 et 2PI
                teta_missile = modulo_teta(teta_missile,0,2 * M_PI);

                //test condition siur zone et distance
                if (comprisentre(teta_missile,teta_1,teta_2))
                {
                    if ((scan.zone_distance[zone_nb-1] > dist_missile) && (dist_robot <= SCAN_DIST_MAX))
                    {
                        scan.zone_distance[zone_nb-1] = dist_missile;
                        if (active_debug)
                        {
                            printf("dist_scannée = %f missile %d\n", scan.zone_distance[zone_nb-1],i);
                        }
                    }
                }
            }
        }
	}
	/*
    for (i=0; i<8; i++)
    {
        printf("zone %d = %f\n",i+1,scan.zone_distance[zone_nb-1]) ;
    }
    */
	return scan;
}

scan_zone_result simu_scan_zone(s_robot* tab_robot,s_mur* tab_mur,s_missile* tab_missile, int id, float teta_scan)
{
    int active_debug =0;
	s_wall wall;
	int i = 0;
	float teta_1 = 0;
	float teta_2 = 0;
	float teta_scan_adapted=0;
    scan_zone_result scan;
    float dist_robot = 0;
    float teta_robot=0;
    float dist_missile = 0;
    float teta_missile=0;

	scan.dist_scan[0] = 1000000;
    // Adaptation teta_scan entre 0 et 2PI
    teta_scan_adapted = teta_scan;
    teta_scan_adapted = modulo_teta(teta_scan_adapted,0, 2*M_PI);

    //calcul de 2 angles extérieurs de la zone
	teta_1 = teta_scan_adapted - (RANGE_SCAN/180.0*M_PI/2);
	teta_2 = teta_scan_adapted + (RANGE_SCAN/180.0*M_PI/2);

    //Test des murs
	for (i = 0; i <  NB_WALL_ALLOWED; i++) //NB_WALL_ALLOWED
	{
		if (tab_mur[i].exist == 1)
		{
			wall_nearest_point(tab_robot, tab_mur, id, i, &wall,teta_1,teta_2);

			//Analyse du point le plus proche
			if (scan.dist_scan[0] > wall.dist_proche)
			{
				scan.dist_scan[0] = wall.dist_proche;
				scan.type_scan[0] = 1; //mur
				scan.x = wall.xp;
				scan.y = wall.yp;
                if (active_debug)
                {
                    //tab_robot[5].x = wall.xp;
                    //tab_robot[5].y = wall.yp;
                    //tab_robot[4].x = wall.xp2;
                    //tab_robot[4].y = wall.yp2;
                    printf("dist_scannée = %f mur %d , xp = %f , yp = %f,type %d\n\n", scan.dist_scan[0],i,wall.xp,wall.yp,scan.type_scan[0]);
                }
			}
		}
	}

	//scan des robots
	for (i = 0; i < NB_ROBOT_MAX; i++)
	{
        if ((i != id) && (tab_robot[i].exist ==1)) //scan uniquement si différent du robot qui scanne et existant
        {
            dist_robot = sqrt(pow(tab_robot[i].x - tab_robot[id].x,2) + pow(tab_robot[i].y - tab_robot[id].y,2)) - RAYON_ROBOT;
            teta_robot = atan2((tab_robot[i].y - tab_robot[id].y), (tab_robot[i].x - tab_robot[id].x));
            //Adaptation teta_robot entre 0 et 2PI
            teta_robot = modulo_teta(teta_robot,0,2 * M_PI);

            //printf("dist_robot%d = %f et teta = %f\n",i,dist_robot,teta_robot);
            if (comprisentre(teta_robot,teta_1,teta_2))
            {
                if (scan.dist_scan[0] > dist_robot)
                {
                    scan.dist_scan[0] = dist_robot;
                    scan.type_scan[0] = 2; //robot
                    scan.x = tab_robot[i].x;
                    scan.y = tab_robot[i].y;
                    if (active_debug)
                    {
                        printf("dist_scannée = %f robot %d\n", scan.dist_scan[0],scan.type_scan[0]);
                    }
                }
            }
        }
	}

    //scan des missiles
	for (i = 0; i < NB_ROBOT_MAX * NB_MISSILE_ALLOWED; i++)
	{
        if ((tab_missile[i].exist == 1)) //scan uniquement si missile existe
        {
            dist_missile = sqrt(pow(tab_missile[i].x - tab_robot[id].x,2) + pow(tab_missile[i].y - tab_robot[id].y,2));
            teta_missile = atan2((tab_missile[i].y - tab_robot[id].y), (tab_missile[i].x - tab_robot[id].x));
            //Adaptation missile entre 0 et 2PI
            teta_missile = modulo_teta(teta_missile,0,2 * M_PI);

            if (comprisentre(teta_missile,teta_1,teta_2))
            {
                if (scan.dist_scan[0] > dist_missile)
                {
                    scan.dist_scan[0] = dist_missile;
                    scan.type_scan[0] = 3; //missile
                    scan.x = tab_missile[i].x;
                    scan.y = tab_missile[i].y;
                    if (active_debug)
                    {
                        printf("dist_scannée = %f missile %d\n", scan.dist_scan[0],scan.type_scan[0]);
                    }
                }
            }
        }
	}
//printf("fin scan\n\n");
return scan;
}

//gestion du déplacement des robots
//calcul position robot, puis meme calcul en fct des murs, Puis recalcul en fct des autres robots et murs

void simu_robot_update(s_robot* tab_robot, s_mur* tab_mur)
{
	int i=0;
	int j=0;
	int k=0;
	float x_temp=0;
	float y_temp=0;
	float pos_diff=0;
	s_wall wall;
	s_wall wall_rob1;
	s_wall wall_rob2;
	float angle = 0;
    float angle2 = 0;

	for (i=0; i < NB_ROBOT_MAX; i++)
	{
		if (tab_robot[i].exist == 1)
		{
            //Calcul de la nouvelle position
			x_temp = tab_robot[i].x + tab_robot[i].speed * cos(tab_robot[i].teta);
			y_temp = tab_robot[i].y + tab_robot[i].speed * sin(tab_robot[i].teta);

			//test des limites du terrain en x
			if (x_temp >= LARGEUR_TERRAIN - RAYON_ROBOT)
				tab_robot[i].x = LARGEUR_TERRAIN - RAYON_ROBOT;
			else if (x_temp <= 0 + RAYON_ROBOT)
				tab_robot[i].x = 0 + RAYON_ROBOT;
			else
				tab_robot[i].x = x_temp;

			//test des limites du terrain en y
			if (y_temp >= HAUTEUR_TERRAIN - RAYON_ROBOT)
				tab_robot[i].y = HAUTEUR_TERRAIN - RAYON_ROBOT;
			else  if (y_temp <= 0 + RAYON_ROBOT)
				tab_robot[i].y = 0 + RAYON_ROBOT;
			else
				tab_robot[i].y = y_temp;

			//gestion des autres murs
			for (j=4; j < NB_WALL_ALLOWED; j++)
			{
				if (tab_mur[j].exist == 1 && tab_mur[j].hv != 3) //gestion des murs avec une pente non nulle, ni horizontal ni verticale
				{
					//calcul des paramètres utiles pour le mur et le robot
					wall_data(tab_robot, tab_mur, i, j, &wall); //zone = 0 => pas de calcul de zone et point le plus proche (scaner)
					pos_diff = wall.d_RI;	//Calcul de la perpendiculaire

                    //repousse le robot si dist à la droite du mur < au rayon du robot et robot compris dans le segment du mur
					if ((pos_diff < RAYON_ROBOT))//&& (wall.teta_i > wall.teta_f)
					{

						if (comprisentre(wall.teta_i,wall.teta_d, wall.teta_f))
						{
						//repousse les robots de la perpendiculaire du mur (kr)
                            //Calcul pour reobot au dessus du mur avec pente positive ou en dessous avec pente <= a 0 ou mur verticale et robot à gauche
							if  ((tab_robot[i].y > wall.yi && tab_mur[j].km > 0) || (tab_robot[i].y < wall.yi && tab_mur[j].km <= 0) || ((tab_mur[j].hv == 1) && tab_robot[i].x < wall.xi))
							{
								tab_robot[i].x -= (RAYON_ROBOT-pos_diff) * cos(atan(tab_mur[j].kr));
								tab_robot[i].y -= (RAYON_ROBOT-pos_diff) * sin(atan(tab_mur[j].kr));
							}
							else
							{
								tab_robot[i].x += (RAYON_ROBOT-pos_diff) * cos(atan(tab_mur[j].kr));
								tab_robot[i].y += (RAYON_ROBOT-pos_diff) * sin(atan(tab_mur[j].kr));
							}
						}
                        //printf("i = %d ,x= %f , y = %f\n", i,tab_robot[i].x,tab_robot[i].y);
					}
				}
			}
		}
	}


/***************************Gestion collission entre 2 robots**************/
	for (i=0; i < NB_ROBOT_MAX; i++)
	{
		if (tab_robot[i].exist ==1)
		{
			for (j=0; j < NB_ROBOT_MAX; j++)
			{
				if ((tab_robot[j].exist == 1) && (i!=j))
				{
					pos_diff = sqrt(pow(tab_robot[i].x - tab_robot[j].x,2) + pow(tab_robot[i].y - tab_robot[j].y,2));
                    //printf("posdiff = %f\n",pos_diff);

                    //robots confondus ?
					if( pos_diff < (2 * RAYON_ROBOT))
					{
                        //calcul de l'angle qui les séparent et repositionnement
                        angle = atan2(tab_robot[j].y-tab_robot[i].y, tab_robot[j].x-tab_robot[i].x);
                        angle2 = atan2(tab_robot[i].y-tab_robot[j].y, tab_robot[i].x-tab_robot[j].x);
                        tab_robot[i].x -= ((2*RAYON_ROBOT-pos_diff)/2) * cos(angle);
                        tab_robot[i].y -= ((2*RAYON_ROBOT-pos_diff)/2) * sin(angle);
                        tab_robot[j].x -= ((2*RAYON_ROBOT-pos_diff)/2) * cos(angle2);
                        tab_robot[j].y -= ((2*RAYON_ROBOT-pos_diff)/2) * sin(angle2);

                        float dist_repousse = 0;
                        // si un des 2 robots dans le mur alors repousses les 2 de la perpendiculaire du mur
                        for (k=0; k < NB_WALL_ALLOWED; k++)
                        {
                            if (tab_mur[k].exist == 1) //gestion des murs avec une pente non nulle, ni horizontal ni verticale
                            {
                                //calcul des paramètres utiles pour le mur et les 2 robots
                                wall_data(tab_robot, tab_mur, i, k, &wall_rob1);
                                wall_data(tab_robot, tab_mur, j, k, &wall_rob2);
                                pos_diff = sqrt(pow(tab_robot[i].x - tab_robot[j].x,2) + pow(tab_robot[i].y - tab_robot[j].y,2));
                                if( pos_diff < (2 * RAYON_ROBOT))
                                {
                                    if ((comprisentre(wall_rob1.teta_i,wall_rob1.teta_d, wall_rob1.teta_f) && (wall_rob1.d_RI < RAYON_ROBOT)) || (((comprisentre(wall_rob2.teta_i,wall_rob2.teta_d, wall_rob2.teta_f)) && (wall_rob2.d_RI < RAYON_ROBOT))))
                                    {
                                    if (comprisentre(wall_rob1.teta_i,wall_rob1.teta_d, wall_rob1.teta_f) && (wall_rob1.d_RI < RAYON_ROBOT))
                                        dist_repousse = wall_rob1.d_RI;
                                    else
                                        dist_repousse = wall_rob2.d_RI;
                                   // printf("________________\n");
                                    //repousse les robots de la perpendiculaire du mur (kr) en fonction de leurs positions par rapport aux murs (memes conditions que robot et mur seul)
                                        if (((tab_robot[i].y > wall_rob1.yi && tab_mur[k].km > 0) || (tab_robot[i].y < wall_rob1.yi && tab_mur[k].km <= 0) || ((tab_mur[k].hv == 1) && tab_robot[i].x < wall_rob1.xi)) || ((tab_robot[j].y > wall_rob2.yi && tab_mur[k].km > 0) || (tab_robot[j].y < wall_rob2.yi && tab_mur[k].km <= 0) || ((tab_mur[k].hv == 1) && tab_robot[j].x < wall_rob2.xi)))
                                        {
                                            tab_robot[i].x -= (RAYON_ROBOT-dist_repousse) * cos(atan(tab_mur[k].kr));
                                            tab_robot[i].y -= (RAYON_ROBOT-dist_repousse) * sin(atan(tab_mur[k].kr));
                                            tab_robot[j].x -= (RAYON_ROBOT-dist_repousse) * cos(atan(tab_mur[k].kr));
                                            tab_robot[j].y -= (RAYON_ROBOT-dist_repousse) * sin(atan(tab_mur[k].kr));
                                        }
                                        else
                                        {
                                            tab_robot[i].x += (RAYON_ROBOT-dist_repousse) * cos(atan(tab_mur[k].kr));
                                            tab_robot[i].y += (RAYON_ROBOT-dist_repousse) * sin(atan(tab_mur[k].kr));
                                            tab_robot[j].x += (RAYON_ROBOT-dist_repousse) * cos(atan(tab_mur[k].kr));
                                            tab_robot[j].y += (RAYON_ROBOT-dist_repousse) * sin(atan(tab_mur[k].kr));
                                        }
                                    }
                                }
                            }
                        }
					}
				}
			}
		}
	}
}


/*permet au robot de tirer un missile dans une direction donnée
prend en parametre:
    - l'angle de tir en radian (float)
    - ne renvoie rien*/
void simu_missile_shoot(s_robot* tab_robot, int id, s_missile* tab_missile, float teta)
{
    //Missile encore disponible pour le robot?
	if ((tab_robot[id].nb_missile < NB_MISSILE_ALLOWED) && (tab_robot[id].exist == 1)) //verif nb missiles tirés
	{
		int i=0;
		//creation d'un missile dans le tableau de missiles
		for (i=0; i < (NB_MISSILE_ALLOWED * NB_ROBOT_MAX);i++) //creation d'un nouveau missile dans premier element s_misile libre
		{
			if (tab_missile[i].exist == 0)
			{
				tab_missile[i].exist = 1; //creation du missile
				tab_robot[id].nb_missile++; //comptage des missiles tirés
				tab_robot[id].teta_tir = teta;
				tab_missile[i].teta = teta;
                //printf("i = %d\n",i);
                //printf("teta missile = %f\n",teta);

				//creation du missile à 1/1000 du rayon pour eviter de pouvoir traverser un mur si robot collé à ce mur
				tab_missile[i].x = tab_robot[id].x + (RAYON_ROBOT*0.999) * cos(tab_missile[i].teta);
                tab_missile[i].y = tab_robot[id].y + (RAYON_ROBOT*0.999) * sin(tab_missile[i].teta);
                tab_missile[i].id_shooter = id;
                //printf("x = %f\n",tab_missile[i].x);
                //printf("cos = %f\n",cos(tab_missile[i].teta));
                //printf("y = %f\n",tab_missile[i].y);
                //printf("sin = %f\n",sin(tab_missile[i].teta));
				//printf("missile pos : x = %f , y : %f\n",tab_missile[i].x, tab_missile[i].y);
                break;
			}
		}
	}
}

//gestion des missiles tirés pendant la simulation
void simu_missile_update(s_missile* tab_missile, s_robot* tab_robot, s_mur* tab_mur,s_impact* tab_impact, context_video* context)
{
	int i=0;
	int j=0;
	float dist_mis_rob=0; //distance between a missile and a robot
	char log[200];
	float missile_previous_x;
	float missile_previous_y;

	for (i=0; i < (NB_ROBOT_MAX * NB_MISSILE_ALLOWED); i++)
	{
		if (tab_missile[i].exist == 1)
		{
			//calcul de la position temporaire
			missile_previous_x = tab_missile[i].x;
			missile_previous_y = tab_missile[i].y;
			tab_missile[i].x = tab_missile[i].x + MISSILE_SPEED * cos(tab_missile[i].teta);
			tab_missile[i].y = tab_missile[i].y + MISSILE_SPEED * sin(tab_missile[i].teta);

		/***************************Check collision between missile and wall**************/

/*
		if (tab_missile[i].exist == 1)	//check if missile still exists (or touch robot)
		{

					//verif des 4 murs du terrain
			if (tab_missile[i].x < 0 || tab_missile[i].x > LARGEUR_TERRAIN || tab_missile[i].y < 0 || tab_missile[i].y > HAUTEUR_TERRAIN)
			{
				tab_missile[i].exist = 0; //Missile suppression
				tab_robot[tab_missile[i].id_shooter].nb_missile--;	//decrementation nb missiles tirés par le joueur
			}
		}
*/
			for (j = 0; j < NB_WALL_ALLOWED ;j++)
			{
				if ((tab_mur[j].exist == 1) && tab_mur[j].hv != 3) // tous les murs sauf verticaux
				{
                    //ancien algo : missile compris dans segment murs et missile traverse mur (y missile change de signe par rapport à intersection)
					//if (comprisentre(tab_missile[i].x,tab_mur[j].Xdebut,tab_mur[j].Xfin) ||(comprisentre(tab_missile[i].y,tab_mur[j].Ydebut,tab_mur[j].Yfin)))
					//{
					//	if ( ( ( (((tab_mur[j].km * tab_missile[i].x + tab_mur[j].b) - tab_missile[i].y) <= 0) && (((tab_mur[j].km * missile_previous_x + tab_mur[j].b) - missile_previous_y) > 0)) || ( ( (((tab_mur[j].km * tab_missile[i].x + tab_mur[j].b) - tab_missile[i].y) >= 0) ) && (((tab_mur[j].km * missile_previous_x + tab_mur[j].b) - missile_previous_y) < 0)) ) )
					//	{

							float k_miss = 0;
							float b_miss = 0;
							float x_impact = 0;
							float y_impact = 0;
                            float angle1 =0;
                            float angle2 =0;

							//calcul des coordonnées de l'impact
							k_miss = (tab_missile[i].y - missile_previous_y) / (tab_missile[i].x - missile_previous_x);
                            b_miss = missile_previous_y - k_miss * missile_previous_x;
                            //printf("droite missile = %f x + %f\n",k_miss,b_miss);
                            x_impact = (b_miss - tab_mur[j].b) / (tab_mur[j].km - k_miss);
                            y_impact = k_miss * x_impact + b_miss;

                            //Adaptation ei missile horizontal
                            if (tab_missile[i].y == missile_previous_y)
                            {
                                y_impact = tab_missile[i].y;
                                x_impact = (y_impact - tab_mur[j].b) / tab_mur[j].km;
                            }

                            //Adaptation si missile vertical
                            if (tab_missile[i].x == missile_previous_x)
                            {
                                x_impact = tab_missile[i].x;
                                y_impact = tab_mur[j].km * x_impact + tab_mur[j].b;
                            }

                            //Adaptation si mur vertical
                            if (tab_mur[j].hv == 1)
                            {
                                x_impact = tab_mur[j].Xdebut;
                                y_impact = k_miss * x_impact + b_miss;
                            }

                            //test si point intersection de la droite du mur est compris dans le mur
                            if (comprisentre(x_impact,tab_mur[j].Xdebut,tab_mur[j].Xfin) && comprisentre(y_impact,tab_mur[j].Ydebut,tab_mur[j].Yfin))
                            {
                                angle1 = atan2(missile_previous_y - y_impact, missile_previous_x - x_impact);
                                angle2 = atan2(tab_missile[i].y - y_impact, tab_missile[i].x - x_impact);
                                //printf("mur = %d angle1 = %f, angle2 = %f\n",j,angle1,angle2);
                                //printf("x_prev=%f , y_prev = %f\n", missile_previous_x,missile_previous_y);
                                //printf("x_=%f , y_ = %f\n", tab_missile[i].x,tab_missile[i].y);

                                //nouvelle algo : test si l'angle entre missile et intersection mur change de signe
                                if (((angle1 <= 0) && (angle2 > 0)) || ((angle1 > 0) && (angle2 <= 0)))
                                {
                                //printf("xi=%f , yi = %f\n", x_impact,y_impact);
                                    tab_missile[i].exist = 0; //Missile suppression
                                    tab_robot[tab_missile[i].id_shooter].nb_missile--;	//decrementation nb missiles tirés par le joueur
                                    missile_impact(tab_impact,x_impact,y_impact);   //gestion de l'impact du missile
                                    wall_destroy(tab_mur, j, x_impact,y_impact);    //et destruction si autorisé
                                }
                            }
						//}
					//}
				}

				//Pour ancien algo
				/*else if ((tab_mur[j].exist == 1) && tab_mur[j].hv == 1) //murs verticaux
				{
					if (comprisentre(tab_missile[i].y,tab_mur[j].Ydebut,tab_mur[j].Yfin))
					{
						if ( ((tab_missile[i].x >= tab_mur[j].Xdebut) && (missile_previous_x < tab_mur[j].Xdebut)) || ((tab_missile[i].x <= tab_mur[j].Xdebut) && (missile_previous_x > tab_mur[j].Xdebut)) )
						{
                            missile_impact(tab_impact,missile_previous_x,missile_previous_y);
							tab_missile[i].exist = 0; //Missile suppression
							tab_robot[tab_missile[i].id_shooter].nb_missile--;	//decrementation nb missiles tirés par le joueur
						}
					}
				}*/
			}
		}


			/***************************Check collision between missile and robot**************/
		if (tab_missile[i].exist == 1)	//check if missile still exists (or touch wall)
		{
			for (j = 0; j < NB_ROBOT_MAX;j++)
			{
                //calcul de la distance entre le missile et le robot si le robot exist
				if ((tab_robot[j].exist == 1) && (tab_missile[i].id_shooter !=j))
				{
					dist_mis_rob = sqrt(pow(tab_missile[i].x - tab_robot[j].x,2) + pow(tab_missile[i].y - tab_robot[j].y,2));

					//robot touche par le missile
					if (dist_mis_rob < RAYON_ROBOT)
					{
                        missile_impact(tab_impact,tab_missile[i].x,tab_missile[i].y);
						tab_missile[i].exist = 0; //Missile suppression
						tab_robot[tab_missile[i].id_shooter].nb_missile--;	//decrementation nb missiles tirés par le joueur
						tab_robot[j].life_point -= 10;

                        //Affichage des informations utiles (qui a touche quoi et robot mort)
                        sprintf(log,"robot %s a ete touche par le missile du joueur %s",tab_robot[j].name,tab_robot[tab_missile[i].id_shooter].name);
                        logger(context,log);
                        sprintf(log,"vie du joueur %s = %d",tab_robot[j].name, tab_robot[j].life_point);
                        logger(context,log);

                        //Check Robot mort
                        if (tab_robot[j].life_point <= 0)
                        {
                            tab_robot[j].count_explosion = 10;
                            tab_robot[j].exist = 0; //robot suppression
                            sprintf(log,"robot %s mort tue par joueur %s!!!!!!!",tab_robot[j].name,tab_robot[tab_missile[i].id_shooter].name	);
                            logger(context,log);
                        }
					}
				}
			}
		}
	}
	//printf("pos x missile 0 = %f \n",tab_missile[0].x);
	//printf("pos y missile 0 = %f \n",tab_missile[0].y);
	//printf("pos x missile 1 = %f \n",tab_missile[1].x);
	//printf("pos y missile 1 = %f \n",tab_missile[1].y);
}

void simu_move_robot(s_robot* tab_robot, int id, float speed, float teta)
{
	tab_robot[id].speed = speed;
	if (speed > ROBOT_MAX_SPEED)
        tab_robot[id].speed = ROBOT_MAX_SPEED;

	tab_robot[id].teta = teta;
	// Adapataion angle robot en tre 0 et 2PI
	tab_robot[id].teta = modulo_teta(tab_robot[id].teta,0, 2*M_PI);

}

 /*permet de connaitre le niveau de vie d'un robot
- pas de parametre
- renvoie un entier qui représente le % de vie du robot (0-100)*/
int simu_get_life(s_robot myrobot){
	return myrobot.life_point;
}

int simu_missile_status(s_robot myrobot){
	return myrobot.nb_missile;
}

s_robot simu_get_position(s_robot myrobot){
	return myrobot;
}

