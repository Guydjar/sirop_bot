// Version 3.2 du 11/01/16

#ifndef GRAPHIQUE_H
#define GRAPHIQUE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../Code_C/robot_api.h"

#define FONT 0
#define ORANGE 1
#define WORLD 2
#define BUBBLE 3
#define ROBOT 4

typedef struct
{
	SDL_Texture *message;
	SDL_Rect Rect;
	int w;
	int h;
}message_texture;

typedef struct
{
	SDL_Window *pWindow;
	SDL_Renderer *pRenderer;
	SDL_Texture *pWarZone;
	SDL_Texture *pMissile;
	SDL_Texture *pExplosion;
	SDL_Texture *pTourelle;
	SDL_Texture *pScanFin;
	SDL_Texture *pScanLarge;
	SDL_Texture **pRobot;
	SDL_Texture **pNbRobot;
	TTF_Font **font;
	message_texture **text;
	char tab_log[10][1000];
}context_video;



int init_graphique(context_video* context);

int close_graphique(context_video* context);

void start_screen(context_video* context);

void refresh_screen_init(context_video* context,s_robot *tab_robot,int nbplayeur);

void refresh_screen(s_robot *tab_robot,s_missile *tab_missile,s_mur *tab_mur,s_impact *tab_impact,context_video* context);

void logger(context_video* context, char *str);

void draw_button(context_video* context);

void draw_score_update(context_video* context,s_robot *tab_robot);

void draw_war_zone(context_video* context);

void draw_log_update(context_video* context);

void draw_robot_and_tourelle(context_video* context,s_robot *tab_robot);

void draw_missile(context_video* context,s_missile *tab_missile,s_impact *tab_impact);

void draw_wall(context_video* context,s_mur *tab_mur);

#endif //GRAPHIQUE_H
