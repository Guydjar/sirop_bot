/**
 * \file client.h
 * \brief client file.
 * \author Toute l'équipe FP25
 * \version 3.3
 * \date 15 janvier 2016
 *
 * client files.
 *
 */


#ifndef CLIENT_H_INCLUDED
#define CLIENT_H_INCLUDED

/**
*\fn init_connexion
*\brief connexion initialization
* comments:
* start connexion
*\param nom name of connexion
*\param ip ip address
*\param port port number
*\return int return a number of connexion
*/
int init_connexion(char *nom, char *ip,int port);

/**
*\fn get_position
*\brief return a structure s_robot
* comments:
* return a structure s_robot
*/
s_robot get_position();

/**
*\fn move
*\brief ask to move robot
* comments:
* ask to move robot
*\param speed speed of robot in m/s
*\param teta direction of robot in radian
*/
void move(float speed, float teta);

/**
*\fn get_status
*\brief check state of robot to know if he touch an object (wall, another robot)
* comments:
* return 1 character (8bits) to know zone in contact (bit0 for zone 1 ... bit7 for zone 8
*\return char return 1 character (8bits) to know zone in contact (bit0 for zone 1 ... bit7 for zone 8
*/
char get_status();

/**
*\fn getlife
*\brief check the life level of robot
* comments:
* return an integer wich represente % of robot life (0-100)
*\return int return an integer wich represente % of robot life (0-100)
*/
int getlife();

/**
*\fn scan_allaround_result
*\brief global scan of robot at 360° for barrier in 8 zones and return the most short distance
* comments:
* global scan of robot at 360° for barrier in 8 zones and return the most short distance
*\return pointer return the pointer to array 8 floats (1 per zone) corresponding to the shorter distance
*/
scan_allaround_result scan_allaround();

/**
*\fn scan_zone_result
*\brief thin scanning robot to check obstacles in zone of 30° (12 zones) and return the closer 10 objects
* comments:
* thin scanning robot to check obstacles in zone of 30° (12 zones) and return the closer 10 objects
*\param teta_zone number of zone to scan (1 - 12)
*\return pointer return pointer to structure scan_zone_result (10 closer objects and their types) (0 = no object,1 = wall, 2 = foe robot,3 = missile)
*/
scan_zone_result scan_zone(float teta_zone);

/**
*\fn missile_shoot
*\brief robot shoot missile in specific direction
* comments:
* robot shoot missile in specific direction
*\param teta angle for fire angle in radian (float)
*/
void missile_shoot(float teta);

/**
*\fn missile_status
*\brief tell you about the number of missiles fired without explosion
* comments:
* tell you about the number of missiles fired without explosion
*\return int a integer for fired missiles
*/
int missile_status();

/**
*\fn close_connexion
*\brief close connexion
* comments:
* close connexion
*/
void close_connexion();

#endif // CLIENT_H_INCLUDED
