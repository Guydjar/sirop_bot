// Version 3.2 du 11/01/16

#ifndef CLIENT_H_INCLUDED
#define CLIENT_H_INCLUDED

int init_connexion(char *nom, char *ip,int port);
s_robot get_position();
void move(float speed, float teta);
char get_status();
int getlife();
scan_allaround_result scan_allaround();
scan_zone_result scan_zone(float teta_zone);
void missile_shoot(float teta);
int missile_status();
void close_connexion();

#endif // CLIENT_H_INCLUDED
