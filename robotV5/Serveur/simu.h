// Version 3.2 du 11/01/16


#ifndef SIMU_H
#define SIMU_H

#include "graphique.h"


scan_zone_result simu_scan_zone(s_robot* tab_robot,s_mur* tab_mur,s_missile* tab_missile, int id, float teta_scan);

void simu_move_robot(s_robot* tab_robot, int id, float speed, float teta);

void simu_missile_update(s_missile* tab_missile, s_robot* tab_robot, s_mur* tab_mur, s_impact* tab_impact, context_video* context);

void simu_missile_shoot(s_robot* tab_robot, int id, s_missile* tab_missile,float teta);

void simu_robot_update(s_robot* tab_robot, s_mur* tab_mur);

scan_allaround_result simu_scan_allaround(s_robot* tab_robot,s_mur* tab_mur,s_missile* tab_missile, int id);

s_robot simu_get_position(s_robot myrobot);

int simu_get_life(s_robot myrobot);

int simu_missile_status(s_robot myrobot);

Uint8 simu_get_status(s_robot* tab_robot,s_mur* tab_mur,s_missile* tab_missile, int id);

#endif //SIMU_H
