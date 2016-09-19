// Version 3.2 du 11/01/16

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include "simu.h"

#include "graphique.h"
#include "robot_api.h"
#include "config.h"
#include "semaphore.h"
#include "wall.h"


void init_robot(s_robot *myrobot);
void init_context(context_video* Context);

void init_context(context_video* Context)
{
    int i=0;
    Context->pWindow = NULL;
	Context->pRenderer = NULL;
	Context->pWarZone = NULL;
	Context->pMissile = NULL;
	Context->pExplosion = NULL;
	Context->pTourelle = NULL;
	Context->pScanFin = NULL;
	Context->pScanLarge = NULL;
	Context->pRobot = NULL;
	Context->pRobot = (SDL_Texture**) malloc(NB_ROBOT_MAX*sizeof(SDL_Texture*));
	Context->pNbRobot = NULL;
	Context->pNbRobot = (SDL_Texture**) malloc(NB_ROBOT_MAX*sizeof(SDL_Texture*));
	Context->font = NULL;
	Context->font = (TTF_Font**) malloc(5*sizeof(TTF_Font*));
	Context->text = NULL;
	Context->text = (message_texture**) malloc(5*sizeof(message_texture*));

	// Allocate memory
	for(i=0;i<5;i++)
	{
		Context->text[i] = (message_texture*) malloc(sizeof(message_texture));
		Context->text[i]->message = NULL;
	}
	// Init log
	for(i=0;i<10;i++)
	{
		sprintf(Context->tab_log[i]," ");
	}
}


/***************************************	MAIN	****************************************/
int main(int argc, char **argv)
{
    if( argc < 2  || argc > 2 || !strcmp(argv[1], "-?") ) {
		printf("Use ./simu.out <port>\n");
		exit(0);
	}

	/***************************************	INIT Yoann	****************************************/
	int i=0; // counter

	int tempsPrecedent = 0, tempsActuel = 0;
	int continuer = 1;
	int wait = 1;
	int nb_player = 0;
    char cmd[15];
    int port = 0;
    port = atoi(argv[1]);

	// Create SDL_Event
    SDL_Event event;

	// Variable for shared memory copied
	int copy_flag_w[10] = {}; // all cells of array will be initialized at 0.
	int copy_flag_r[10] = {}; // all cells of array will be initialized at 0.
	char copy_tab_cmd[4000] = "requete"; // all cells of array will be initialized at 0.
	char copy_name[40] = "name";
	int copy_connected[10] = {};

	// ID and ptr, memory and semaphore
	int mem_ID; //	identifier of segment shared memory associeted to CLEF
	int sem_ID; //	identifier of shared semaphore associeted to CLEFSEM
	void* ptr_mem_partagee; //	pointer to segment address of shared memory
	structure_partagee Data; // structure "structure_partagee" of name Data, to design the zone size of shared memory.

	// conversion variables
	s_robot myrobot;
	scan_zone_result scan_zone;
	scan_allaround_result scan_around;
	float speed,teta;
	float teta_scan = 0;
	int vie, nbmissile;
	Uint8 stat;

	// Create shared memory
	if ((mem_ID = shmget(CLEF, sizeof(Data), 0666 | IPC_CREAT)) < 0)
	{	//	create new size of memory segment "taille de ma structure data" in octets, with access in writing and reading
		printf("shmget");											    //	verify memory space
		exit(1);
	}
	if ((ptr_mem_partagee = shmat(mem_ID, NULL, 0)) == (void*) -1)
	{	//	close shared memory of segment identified by mem_ID to data segment of processus A in free zone defined by operating system
		printf("shmat");											//	check memory of segment will be correctly attached in my processus
		exit(1);
	}

	// Create semaphore
	sem_ID = sem_create(1);

	//	Initialisation structure values of shared memory
	for(i = 0 ; i < 10 ; i++)
	{
		((structure_partagee*)ptr_mem_partagee)->flag_w[i] = copy_flag_w[i];
		((structure_partagee*)ptr_mem_partagee)->flag_r[i] = copy_flag_r[i];
		sprintf(((structure_partagee*)ptr_mem_partagee)->tab_cmd[i],"%s",copy_tab_cmd);
		sprintf(((structure_partagee*)ptr_mem_partagee)->name[i],"%s",copy_name);
		((structure_partagee*)ptr_mem_partagee)->connected[i] = copy_connected[i];
	}
	((structure_partagee*)ptr_mem_partagee)->game_status = 0;

	//define array of wall, robots and missiles
	s_mur tab_mur[NB_WALL_ALLOWED] = {};
	s_robot tab_robot[NB_ROBOT_MAX] = {};
	s_missile tab_missile[NB_ROBOT_MAX*NB_MISSILE_ALLOWED] = {};
	s_impact tab_impact[NB_ROBOT_MAX*NB_MISSILE_ALLOWED] = {};

	//for(i=0;i<NB_ROBOT_MAX*NB_MISSILE_ALLOWED;i++)
    //    tab_impact[i].count_impact =0;


	// SDL Window and texture
	context_video Context;
	init_context(&Context);

	init_graphique(&Context);
	start_screen(&Context);

	sprintf(cmd,"./serveur.out %d",port);

	system(cmd);
	sleep(1);

	while(wait)
	{
		// events management
		SDL_PollEvent(&event); /* On utilise PollEvent et non WaitEvent pour ne pas bloquer le programme */
        switch(event.type)
        {
            case SDL_QUIT:
                wait = 0;
                continuer = 0;
                break;

			case SDL_MOUSEBUTTONUP: /* Clic de la souris */

				if(event.button.button == SDL_BUTTON_LEFT)
				{
					// Start button
					if(event.button.x>(LARGEUR_TERRAIN+(2*MARGE_ECRAN)) && event.button.x<(LARGEUR_TERRAIN+(2*MARGE_ECRAN)+LARGEUR_SCORE))
					{
						if(event.button.y>(HAUTEUR_TERRAIN+(2*MARGE_ECRAN)) && event.button.y<(HAUTEUR_TERRAIN+(2*MARGE_ECRAN)+(HAUTEUR_LOG/2)))
						{
							wait = 0;
						}
					}
					// Quit button
					if(event.button.x>(LARGEUR_TERRAIN+(2*MARGE_ECRAN)) && event.button.x<(LARGEUR_TERRAIN+(2*MARGE_ECRAN)+LARGEUR_SCORE))
					{
						if(event.button.y>(HAUTEUR_TERRAIN+(2*MARGE_ECRAN)+(HAUTEUR_LOG/2)) && event.button.y<(HAUTEUR_TERRAIN+(2*MARGE_ECRAN)+HAUTEUR_LOG))
						{
							wait = 0;
							continuer = 0;
						}
					}
				}
				break;
			default:
                    break;

        }
        nb_player = 0;
        down(sem_ID, 0);// lock access memory by semaphore
		for(i=0;i<NB_ROBOT_MAX;i++){
			if(((structure_partagee*)ptr_mem_partagee)->connected[i] == 1)
			{
				nb_player++;
			}
			else
				tab_robot[i].exist = 0;
		}
		up(sem_ID, 0);// free semaphore



        SDL_Delay(5);
        refresh_screen_init(&Context,tab_robot,nb_player);
	}



	// Init robot connected
	init_robot(tab_robot); // initialisation robot position

	down(sem_ID, 0);// lock shared memory by semaphore
	for(i=0;i<10;i++){
		if(((structure_partagee*)ptr_mem_partagee)->connected[i] == 1)
		{
			tab_robot[i].exist = 1;
			sprintf(tab_robot[i].name,"%s",((structure_partagee*)ptr_mem_partagee)->name[i]);
		}
		else
			tab_robot[i].exist = 0;
	}
	up(sem_ID, 0);// free semaphore


/***************************************	Start global	****************************************/


	//////////////////////////////////////////////////// Variables and commands for flo ////////////////////////////////////////////////////////
	//initialisation of robots
	tab_robot[7].x = 150;
	tab_robot[7].y = 100;
	tab_robot[7].teta = M_PI/4;
	tab_robot[7].life_point = 11;
	tab_robot[7].exist = 1;
	sprintf(tab_robot[7].name,"Arnaud");
	tab_robot[9].x = 200;
	tab_robot[9].y = 350;
	tab_robot[9].teta = -3*M_PI/4;
	tab_robot[9].life_point = 96;
	tab_robot[9].exist = 1;
	sprintf(tab_robot[9].name,"Bernard");
	tab_robot[2].x = 190;
	tab_robot[2].y = 70;
	tab_robot[2].teta = -3*M_PI/4;
	tab_robot[2].life_point = 8;
	tab_robot[2].exist = 0;
	sprintf(tab_robot[2].name,"Scarf");
	tab_robot[4].teta = -3*M_PI/4;
	tab_robot[4].life_point = 8;
	tab_robot[4].exist = 1;
	tab_robot[4].x = 650;
	tab_robot[4].y = 400;
	sprintf(tab_robot[4].name,"Isaac");
	tab_robot[5].x = 750;
	tab_robot[5].y = 580;
	tab_robot[5].teta = M_PI/4;
	tab_robot[5].life_point = 8;
	tab_robot[5].exist = 1;
	sprintf(tab_robot[5].name,"Matteo");
	tab_robot[8].x = 500;
	tab_robot[8].y = 400;
	tab_robot[8].teta = M_PI/3;
	tab_robot[8].life_point = 100;
	tab_robot[8].exist = 1;
	sprintf(tab_robot[8].name,"toto");


	simu_move_robot(tab_robot,7,0.8,135*M_PI/180);
	simu_move_robot(tab_robot,9,1,-M_PI/2);
	simu_move_robot(tab_robot,2,1,90*M_PI/180);

	simu_move_robot(tab_robot,8,0,0);
	simu_missile_shoot(tab_robot, 0, tab_missile,0);
	simu_missile_shoot(tab_robot, 1, tab_missile,-M_PI);

	//wall initialisation
	creer_mur(tab_mur,0,0,0,HAUTEUR_TERRAIN);
	creer_mur(tab_mur,0,HAUTEUR_TERRAIN,LARGEUR_TERRAIN,HAUTEUR_TERRAIN);
	creer_mur(tab_mur,LARGEUR_TERRAIN,HAUTEUR_TERRAIN,LARGEUR_TERRAIN,0);
	creer_mur(tab_mur,LARGEUR_TERRAIN,0,0,0);

	creer_mur(tab_mur,150,150,250,250);
	creer_mur(tab_mur,600,570,800,390);
	creer_mur(tab_mur,25,450,300,350);
	creer_mur(tab_mur,400,200,550,100);
	creer_mur(tab_mur,0,100,200,100);
	creer_mur(tab_mur,500,450,500,350);
	creer_mur(tab_mur,400,0,400,300);



	// Robot manual
	float vitesse_man = 0;

////////////////////////////////////////////////////// commands flo finish /////////////////////////////////////////////////////////////////////


while(continuer)
	{
	// events management
		//SDL_PollEvent(&event); /* we use PollEvent and no WaitEvent to lock the program */
        while (SDL_PollEvent(&event))
        switch(event.type)
        {
            case SDL_QUIT:
                continuer = 0;
                break;
			case SDL_MOUSEBUTTONUP: /* Clic de la souris */
				if(event.button.button == SDL_BUTTON_LEFT)
				{
					if(event.button.x>(LARGEUR_TERRAIN+(2*MARGE_ECRAN)) && event.button.x<(LARGEUR_TERRAIN+(2*MARGE_ECRAN)+LARGEUR_SCORE))
					{
						if(event.button.y>(HAUTEUR_TERRAIN+(2*MARGE_ECRAN)+(HAUTEUR_LOG/2)) && event.button.y<(HAUTEUR_TERRAIN+(2*MARGE_ECRAN)+HAUTEUR_LOG))
						{
							continuer = 0;
						}
					}
				}
				break;
			case SDL_KEYDOWN:
				switch(event.key.keysym.sym)
				{

					case SDLK_o:
						simu_move_robot(tab_robot,8,vitesse_man,tab_robot[8].teta-0.1);
					break;
					case SDLK_p:
						simu_move_robot(tab_robot,8,vitesse_man,tab_robot[8].teta+0.1);
					break;
					case SDLK_x:
						teta_scan = teta_scan + 10.0/180.0*M_PI;
						printf("teta_scan = %f\n",teta_scan);
					break;
					case SDLK_w:
						teta_scan = teta_scan - 10.0/180.0*M_PI;
						printf("teta_scan = %f\n",teta_scan);
					break;
					case SDLK_r:
						tab_robot[8].scan_zone = simu_scan_zone(tab_robot, tab_mur, tab_missile,8, teta_scan);
						tab_robot[8].teta_scan = teta_scan;
						tab_robot[8].count_scan = 10;
						printf("dist_scannée = %f ,type %d\n", scan_zone.dist_scan[0],scan_zone.type_scan[0]);
					break;
					case SDLK_m:
						scan_around = simu_scan_allaround(tab_robot, tab_mur,tab_missile,8);
						tab_robot[8].count_scanLarge = 10;
for (i=0; i<8; i++)
{
	printf("zone %d = %f\n",i+1,scan_around.zone_distance[i]);
}
					break;
                    case SDLK_c:
						stat = simu_get_status(tab_robot,tab_mur,tab_missile,8);
						printf("status_result=%d\n",stat);
					break;

					case SDLK_z:
						simu_move_robot(tab_robot,8,vitesse_man,M_PI/2);
					break;
					case SDLK_s:
						simu_move_robot(tab_robot,8,vitesse_man,-M_PI/2);
					break;
					case SDLK_q:
						simu_move_robot(tab_robot,8,vitesse_man,M_PI);
					break;
					case SDLK_d:
						simu_move_robot(tab_robot,8,vitesse_man,0);
					break;
					case SDLK_UP:
						simu_missile_shoot(tab_robot, 8, tab_missile,M_PI/4);
					break;
					case SDLK_DOWN:
						simu_missile_shoot(tab_robot, 8, tab_missile,3*M_PI/4);
					break;
					case SDLK_LEFT:
						simu_missile_shoot(tab_robot, 8, tab_missile,-M_PI/4);
					break;
					case SDLK_RIGHT:
						simu_missile_shoot(tab_robot, 8, tab_missile,-3*M_PI/4);
					break;
					case SDLK_t:
						vitesse_man = 0;
					break;
					case SDLK_y:
						vitesse_man = 1;
					break;
					case SDLK_u:
						vitesse_man = 2;
					break;
					case SDLK_i:
						vitesse_man = 3;
					break;
					default:
                    break;

				}
			break;
			default:
				break;
        }

        //timing management
		tempsActuel = SDL_GetTicks();
		//printf("Temps : %d\n",tempsActuel - tempsPrecedent);
        if (tempsActuel - tempsPrecedent > 20) /* Si 30 ms se sont écoulées depuis le dernier tour de boucle */
        {
			down(sem_ID, 0);// lock access for shared memory with semaphore
			//	copy of writing and reading flags
			for(i = 0 ; i < 10 ; i++)
			{
				copy_flag_r[i] = 0;
				copy_flag_w[i] = ((structure_partagee*)ptr_mem_partagee)->flag_w[i];
			}
			for(i=0;i<10;i++)
			{
				if(copy_flag_w[i] == 1 && copy_flag_r[i] == 0  )
				{
					switch(((structure_partagee*)ptr_mem_partagee)->tab_cmd[i][0])
					{
						case 'A':
							myrobot = simu_get_position(tab_robot[i]);
							sprintf(((structure_partagee*)ptr_mem_partagee)->tab_cmd[i],"%f;%f;%f;%d;",myrobot.x, myrobot.y, myrobot.teta, myrobot.exist);
							break;
						case 'B' :
							sscanf(((structure_partagee*)ptr_mem_partagee)->tab_cmd[i],"B;%f;%f;",&speed,&teta);
							simu_move_robot(tab_robot,i,speed,teta);
							sprintf(((structure_partagee*)ptr_mem_partagee)->tab_cmd[i],"move OK;");
							break;
						case 'C' :
							stat = simu_get_status(tab_robot,tab_mur,tab_missile,i);
							sprintf(((structure_partagee*)ptr_mem_partagee)->tab_cmd[i],"%d;",stat);
							break;
						case 'D' :
							vie = simu_get_life(tab_robot[i]);
							sprintf(((structure_partagee*)ptr_mem_partagee)->tab_cmd[i],"%d;",vie);
							break;
						case 'E' :
							scan_around = simu_scan_allaround(tab_robot,tab_mur,tab_missile,i);
							tab_robot[i].count_scanLarge = 10;
							sprintf(((structure_partagee*)ptr_mem_partagee)->tab_cmd[i],"%f;%f;%f;%f;%f;%f;%f;%f;",scan_around.zone_distance[0],scan_around.zone_distance[1],scan_around.zone_distance[2],scan_around.zone_distance[3],scan_around.zone_distance[4],scan_around.zone_distance[5],scan_around.zone_distance[6],scan_around.zone_distance[7]);
							break;
						case 'F' :
							sscanf(((structure_partagee*)ptr_mem_partagee)->tab_cmd[i],"F;%f;",&teta_scan);
							tab_robot[i].scan_zone = simu_scan_zone(tab_robot, tab_mur,tab_missile,i,teta_scan);
							tab_robot[i].teta_scan = teta_scan;
							tab_robot[i].count_scan = 10;
							sprintf(((structure_partagee*)ptr_mem_partagee)->tab_cmd[i],"%f;%d;",scan_zone.dist_scan[0],scan_zone.type_scan[0]);
							break;
						case 'G' :
							sscanf(((structure_partagee*)ptr_mem_partagee)->tab_cmd[i],"G;%f;",&teta);
							simu_missile_shoot(tab_robot,i,tab_missile,teta);
							sprintf(((structure_partagee*)ptr_mem_partagee)->tab_cmd[i],"Missile shoot OK;");
							break;
						case 'H' :
							nbmissile = simu_missile_status(tab_robot[i]);
							sprintf(((structure_partagee*)ptr_mem_partagee)->tab_cmd[i],"%d;",nbmissile);
							break;
						case 'Z' :
							tab_robot[i].exist = 0;
							((structure_partagee*)ptr_mem_partagee)->connected[i] = 0;

							break;
					}
					copy_flag_r[i] = 1;
					copy_flag_w[i] = 0;
					printf("client %d : %d,%d\n%s\n",i,copy_flag_r[i],copy_flag_w[i],((structure_partagee*)ptr_mem_partagee)->tab_cmd[i]);
				}
			}
			// copies flags after treatment
			for(i = 0 ; i < 10 ; i++)
			{
				if(copy_flag_r[i] == 1)
				{
					((structure_partagee*)ptr_mem_partagee)->flag_r[i] = copy_flag_r[i];
					((structure_partagee*)ptr_mem_partagee)->flag_w[i] = copy_flag_w[i];
				}
			}
			up(sem_ID, 0);// free semaphore

			simu_robot_update(tab_robot, tab_mur);
			simu_missile_update(tab_missile, tab_robot,tab_mur,tab_impact,&Context);
			refresh_screen(tab_robot,tab_missile,tab_mur,tab_impact,&Context);
			tempsPrecedent = tempsActuel;
        }
	}
	// close server
	down(sem_ID, 0);// lock access of shared memory by semaphore
	((structure_partagee*)ptr_mem_partagee)->game_status = 1;
	up(sem_ID, 0);// free semaphore

	//	at the end of the loop, the memory segment is free from the processus, and when all of the processus will
	//  do the same, this emory segment will be deleted.
	shmdt(ptr_mem_partagee);
	sem_delete(sem_ID);

	close_graphique(&Context);
	sleep(1);

	return 1;
}
/***************************************	END MAIN	****************************************/




void init_robot(s_robot *tab_robot){

	//robots initialization
	int i = 0;
	for(i=0;i<NB_ROBOT_MAX;i++)
	{
		tab_robot[i].exist = 0;
		tab_robot[i].count_scan = 0;
		tab_robot[i].count_scanLarge = 0;
		tab_robot[i].life_point = 100;
	}

	tab_robot[0].x = 72;
	tab_robot[0].y = 54;
	tab_robot[0].teta = M_PI/4;

	tab_robot[1].x = 144;
	tab_robot[1].y = 108;
	tab_robot[1].teta = M_PI/4;

	tab_robot[2].x = 216;
	tab_robot[2].y = 162;
	tab_robot[2].teta = M_PI/4;

	tab_robot[3].x = 288;
	tab_robot[3].y = 216;
	tab_robot[3].teta = M_PI/4;

	tab_robot[4].x = 360;
	tab_robot[4].y = 270;
	tab_robot[4].teta = M_PI/4;

	tab_robot[5].x = 432;
	tab_robot[5].y = 324;
	tab_robot[5].teta = M_PI/4;

	tab_robot[6].x = 504;
	tab_robot[6].y = 378;
	tab_robot[6].teta = M_PI/4;

	tab_robot[7].x = 576;
	tab_robot[7].y = 432;
	tab_robot[7].teta = M_PI/4;

	tab_robot[8].x = 648;
	tab_robot[8].y = 486;
	tab_robot[8].teta = M_PI/4;

	tab_robot[9].x = 500;
	tab_robot[9].y = 500;
	tab_robot[9].teta = M_PI/4;
}




