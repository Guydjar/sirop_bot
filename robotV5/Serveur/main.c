// Version 3.6 du 13/11/16

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
#include <time.h>
#include "simu.h"
#include "graphique.h"
#include "robot_simu.h"
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
	Context->text = (message_texture**) malloc(8*sizeof(message_texture*));

	// Allocate memory
	for(i=0;i<8;i++)
	{
		Context->text[i] = (message_texture*) malloc(sizeof(message_texture));
		Context->text[i]->message = NULL;
	}
	// Init log
	for(i=0;i<7;i++)
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
	int debugmode=0;
	int level = 1;
	int nb_player = 0;
    char cmd[15];
    int port = 0;
    port = atoi(argv[1]);

	// Create SDL_Event
    SDL_Event event;

	// Variable pour la copie de la memoire partagee
	int copy_flag_w[10] = {}; // Toutes les cases du tableau seront initialisées à 0
	int copy_flag_r[10] = {}; // Toutes les cases du tableau seront initialisées à 0
	char copy_tab_cmd[4000] = "requete"; // Toutes les cases du tableau seront initialisées à 0
	char copy_name[40] = "name";
	int copy_connected[10] = {};

	// ID and ptr, memory and semaphore
	int mem_ID; //	identificateur du segment de mémoire partagée associé à CLEF
	int sem_ID; //	identificateur du jeux de semaphore partagée associé à CLEFSEM
	void* ptr_mem_partagee; //	pointeur sur l'adresse d'attachement du segment de mémoire partagée
	structure_partagee Data; // une structure "structure_partagee" de nom Data, sert pour dimentionner la zone de memoire partagé.

	// Variable de conversion
	s_robot myrobot;
	float speed,teta;
	float teta_scan = 0;
	int vie, nbmissile;
	Uint8 stat;

	// Create shared memory
	if ((mem_ID = shmget(CLEF, sizeof(Data), 0666 | IPC_CREAT)) < 0)
	{	//	je crée un nouveau segment mémoire de taille "taille de ma structure data" octets, avec des droits d'écriture et de lecture
		printf("shmget");											    //	et je m'assure que l'espace mémoire a été correctement créé
		exit(1);
	}
	if ((ptr_mem_partagee = shmat(mem_ID, NULL, 0)) == (void*) -1)
	{	//	J'attache le segment de mémoire partagée identifié par mem_ID au segment de données du processus A dans une zone libre déterminée par le Système d'exploitation
		printf("shmat");											//	et je m'assure que le segment de mémoire a été correctement attaché à mon processus
		exit(1);
	}

	// Creation du semaphore
	sem_ID = sem_create(1);

	//	Initialisation des valeurs de la structure de memoire partagé
	for(i = 0 ; i < 10 ; i++)
	{
		((structure_partagee*)ptr_mem_partagee)->flag_w[i] = copy_flag_w[i];
		((structure_partagee*)ptr_mem_partagee)->flag_r[i] = copy_flag_r[i];
		sprintf(((structure_partagee*)ptr_mem_partagee)->tab_cmd[i],"%s",copy_tab_cmd);
		sprintf(((structure_partagee*)ptr_mem_partagee)->name[i],"%s",copy_name);
		((structure_partagee*)ptr_mem_partagee)->connected[i] = copy_connected[i];
	}
	((structure_partagee*)ptr_mem_partagee)->game_status = 0;

	//déclaration des tableaux de mur, robot et missile
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
	sleep(3);

	while(wait)
	{
		// Gestion evenement
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
						if(event.button.y>(HAUTEUR_TERRAIN+(MARGE_ECRAN)+(MARGE_ECRAN/2)-(HAUTEUR_LOG/2)) && event.button.y<(HAUTEUR_TERRAIN+(MARGE_ECRAN)))
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

					// LEVEL I button				
					if(event.button.x>(LARGEUR_TERRAIN+(2*MARGE_ECRAN)) && event.button.x<((LARGEUR_TERRAIN)+(2*MARGE_ECRAN)+(LARGEUR_SCORE/2)-(MARGE_ECRAN/2)))
					{
						if(event.button.y>((MARGE_ECRAN)+(HAUTEUR_TERRAIN/2)) && event.button.y<((MARGE_ECRAN)+(HAUTEUR_TERRAIN/2)+(HAUTEUR_LOG/2)-(MARGE_ECRAN/2)))
						{
							level=1;
						}
						
					}
								
					// LEVEL II button			
					if(event.button.x>(LARGEUR_TERRAIN+(2*MARGE_ECRAN)+(MARGE_ECRAN/2)+(LARGEUR_SCORE/2)) && event.button.x<(LARGEUR_TERRAIN+(2*MARGE_ECRAN)+(LARGEUR_SCORE)))
					{
						if(event.button.y>((MARGE_ECRAN)+(HAUTEUR_TERRAIN/2)) && event.button.y<((MARGE_ECRAN)+(HAUTEUR_TERRAIN/2)+(HAUTEUR_LOG/2)-(MARGE_ECRAN/2)))
						{
							level=2;
						}
					}
				
					// LEVEL III button				
					if(event.button.x>(LARGEUR_TERRAIN+(2*MARGE_ECRAN)) && event.button.x<(LARGEUR_TERRAIN+(2*MARGE_ECRAN)+(LARGEUR_SCORE/2)-(MARGE_ECRAN/2)))
					{
						if(event.button.y>((MARGE_ECRAN)+(HAUTEUR_TERRAIN/2)+(HAUTEUR_LOG/2)) && event.button.y<(((MARGE_ECRAN)+(HAUTEUR_TERRAIN/2)+(HAUTEUR_LOG)-(MARGE_ECRAN/2))))
						{
							level=3;
						}
						
					}
				
					// LEVEL IV button
					if(event.button.x>(LARGEUR_TERRAIN+(2*MARGE_ECRAN)+(MARGE_ECRAN/2)+(LARGEUR_SCORE/2)) && event.button.x<(LARGEUR_TERRAIN+(2*MARGE_ECRAN)+(LARGEUR_SCORE)))
					{
						if(event.button.y>((MARGE_ECRAN)+(HAUTEUR_TERRAIN/2)+(HAUTEUR_LOG/2)) && event.button.y<(((MARGE_ECRAN)+(HAUTEUR_TERRAIN/2)+(HAUTEUR_LOG)-(MARGE_ECRAN/2))))
						{
							level=4;
						}
					}

					// DEBUG MODE button
					if(event.button.x>(LARGEUR_TERRAIN+(2*MARGE_ECRAN)) && event.button.x<(LARGEUR_TERRAIN+(2*MARGE_ECRAN)+(LARGEUR_SCORE/2)-(MARGE_ECRAN/2)))
					{
						if(event.button.y>((2*MARGE_ECRAN)+(HAUTEUR_TERRAIN)) && event.button.y<(((2*MARGE_ECRAN)+(HAUTEUR_TERRAIN)+(HAUTEUR_LOG/2)-(MARGE_ECRAN/2))))
						{
							debugmode=1;
							wait = 0;
						}
					}
					// HELP button
					if(event.button.x>(LARGEUR_TERRAIN+(2*MARGE_ECRAN)+(MARGE_ECRAN/2)+(LARGEUR_SCORE/2)) && event.button.x<(LARGEUR_TERRAIN+(2*MARGE_ECRAN)+(LARGEUR_SCORE)))
					{
						if(event.button.y>((2*MARGE_ECRAN)+(HAUTEUR_TERRAIN)) && event.button.y<(((2*MARGE_ECRAN)+(HAUTEUR_TERRAIN)+(HAUTEUR_LOG/2)-(MARGE_ECRAN/2))))
						{
							wait = 0;
						}
					}

				}
				break;
			default:
                    break;

        }
        nb_player = 0;
        down(sem_ID, 0);// on bloque l'acces a la memoire partagee avec le semaphore
		for(i=0;i<NB_ROBOT_MAX;i++){
			if(((structure_partagee*)ptr_mem_partagee)->connected[i] == 1)
			{
				nb_player++;
			}
			else
				tab_robot[i].exist = 0;
		}
		up(sem_ID, 0);// On libere le semaphore



        SDL_Delay(5);
        refresh_screen_init(&Context,tab_robot,nb_player);
	}



	// Init robot connected
	init_robot(tab_robot); // initialisation position robot

	down(sem_ID, 0);// on bloque l'acces a la memoire partagee avec le semaphore
	for(i=0;i<10;i++){
		if(((structure_partagee*)ptr_mem_partagee)->connected[i] == 1)
		{
			tab_robot[i].exist = 1;
			sprintf(tab_robot[i].name,"%s",((structure_partagee*)ptr_mem_partagee)->name[i]);
		}
		else
			tab_robot[i].exist = 0;
	}
	up(sem_ID, 0);// On libere le semaphore


/***************************************	Start global	****************************************/


	//////////////////////////////////////////////////// Variable et commande pour flo ////////////////////////////////////////////////////////
	//initialisation des robots
	switch(debugmode)
  	{
	 	case 0:
			tab_robot[9].exist = 0;

		break;

		case 1:
			tab_robot[9].exist = 1;
			sprintf(tab_robot[9].name,"The beast");
		break;
	}


	/*simu_move_robot(tab_robot,7,0.8,135*M_PI/180);
	simu_move_robot(tab_robot,9,1,-M_PI/2);
	simu_move_robot(tab_robot,2,1,90*M_PI/180);

	simu_move_robot(tab_robot,8,0,0);
	simu_missile_shoot(tab_robot, 0, tab_missile,0);
	simu_missile_shoot(tab_robot, 1, tab_missile,-M_PI);
*/
	//initialisation des murs exterieurs
	creer_mur(tab_mur,0,0,0,HAUTEUR_TERRAIN);
	creer_mur(tab_mur,0,HAUTEUR_TERRAIN,LARGEUR_TERRAIN,HAUTEUR_TERRAIN);
	creer_mur(tab_mur,LARGEUR_TERRAIN,HAUTEUR_TERRAIN,LARGEUR_TERRAIN,0);
	creer_mur(tab_mur,LARGEUR_TERRAIN,0,0,0);

	//initialisation des murs
	srand(time(NULL));
	switch(level)
  	{
	 	case 1:
		//mur level 1
			creer_mur(tab_mur,0,HAUTEUR_TERRAIN/2,LARGEUR_TERRAIN/2,HAUTEUR_TERRAIN/2);
			creer_mur(tab_mur,LARGEUR_TERRAIN/2,HAUTEUR_TERRAIN/4,LARGEUR_TERRAIN,HAUTEUR_TERRAIN/4);
			creer_mur(tab_mur,LARGEUR_TERRAIN/2,3*HAUTEUR_TERRAIN/4,LARGEUR_TERRAIN,3*HAUTEUR_TERRAIN/4);                         
	  	break;

		case 2:
		//mur level 2
			creer_mur(tab_mur,20*LARGEUR_TERRAIN/100,HAUTEUR_TERRAIN,20*LARGEUR_TERRAIN/100,HAUTEUR_TERRAIN/2);
			creer_mur(tab_mur,40*LARGEUR_TERRAIN/100,0,40*LARGEUR_TERRAIN/100,HAUTEUR_TERRAIN/2);
			creer_mur(tab_mur,60*LARGEUR_TERRAIN/100,HAUTEUR_TERRAIN,60*LARGEUR_TERRAIN/100,HAUTEUR_TERRAIN/2);
			creer_mur(tab_mur,80*LARGEUR_TERRAIN/100,0,80*LARGEUR_TERRAIN/100,HAUTEUR_TERRAIN/2);
			break;
	
	 	case 3:
		//mur level 3
			creer_mur(tab_mur,50*LARGEUR_TERRAIN/100,70*HAUTEUR_TERRAIN/100,50*LARGEUR_TERRAIN/100,30*HAUTEUR_TERRAIN/100);
			creer_mur(tab_mur,30*LARGEUR_TERRAIN/100,50*HAUTEUR_TERRAIN/100,70*LARGEUR_TERRAIN/100,50*HAUTEUR_TERRAIN/100);
			creer_mur(tab_mur,15*LARGEUR_TERRAIN/100,70*HAUTEUR_TERRAIN/100,15*LARGEUR_TERRAIN/100,30*HAUTEUR_TERRAIN/100);
			creer_mur(tab_mur,30*LARGEUR_TERRAIN/100,15*HAUTEUR_TERRAIN/100,70*LARGEUR_TERRAIN/100,15*HAUTEUR_TERRAIN/100);
			creer_mur(tab_mur,85*LARGEUR_TERRAIN/100,70*HAUTEUR_TERRAIN/100,85*LARGEUR_TERRAIN/100,30*HAUTEUR_TERRAIN/100);
			creer_mur(tab_mur,30*LARGEUR_TERRAIN/100,85*HAUTEUR_TERRAIN/100,70*LARGEUR_TERRAIN/100,85*HAUTEUR_TERRAIN/100);
		                
	  	break;

		case 4:
		//mur level 4
			creer_mur(tab_mur,0,HAUTEUR_TERRAIN,25*LARGEUR_TERRAIN/100,75*HAUTEUR_TERRAIN/100);
			creer_mur(tab_mur,0,0,25*LARGEUR_TERRAIN/100,25*HAUTEUR_TERRAIN/100);
			creer_mur(tab_mur,LARGEUR_TERRAIN,HAUTEUR_TERRAIN,75*LARGEUR_TERRAIN/100,75*HAUTEUR_TERRAIN/100);
			creer_mur(tab_mur,LARGEUR_TERRAIN,0,75*LARGEUR_TERRAIN/100,25*HAUTEUR_TERRAIN/100);
		
			creer_mur(tab_mur,0,50*HAUTEUR_TERRAIN/100,30*LARGEUR_TERRAIN/100,50*HAUTEUR_TERRAIN/100);
			creer_mur(tab_mur,LARGEUR_TERRAIN,50*HAUTEUR_TERRAIN/100,70*LARGEUR_TERRAIN/100,50*HAUTEUR_TERRAIN/100);
			creer_mur(tab_mur,50*LARGEUR_TERRAIN/100,HAUTEUR_TERRAIN,50*LARGEUR_TERRAIN/100,75*HAUTEUR_TERRAIN/100);
			creer_mur(tab_mur,50*LARGEUR_TERRAIN/100,0,50*LARGEUR_TERRAIN/100,25*HAUTEUR_TERRAIN/100);

			creer_mur(tab_mur,35*LARGEUR_TERRAIN/100,65*HAUTEUR_TERRAIN/100,65*LARGEUR_TERRAIN/100,35*HAUTEUR_TERRAIN/100);
			creer_mur(tab_mur,65*LARGEUR_TERRAIN/100,65*HAUTEUR_TERRAIN/100,35*LARGEUR_TERRAIN/100,35*HAUTEUR_TERRAIN/100);
		
		
		break;
		default:
    	break;
  	}

/*    int x_all,y_all,x_fin_all,y_fin_all;
    //Murs aléatoires distance et pente 100% aléatoire
    if (DIST_WALL_INIT == -1)
    {
        for(i=0;i<NB_WALL_INIT;i++)
        {
            creer_mur(tab_mur,rand() % LARGEUR_TERRAIN, rand() % HAUTEUR_TERRAIN, rand() % LARGEUR_TERRAIN,rand() % HAUTEUR_TERRAIN);
        }
    }
    //Autant de pentes positives que negatives avec distance limitée
    else
    {
        for(i=0;i<NB_WALL_INIT/2;i++)
        {
            x_fin_all = rand() % DIST_WALL_INIT;
            y_fin_all = rand() % DIST_WALL_INIT;
            x_all = rand() % LARGEUR_TERRAIN;
            y_all = rand() % HAUTEUR_TERRAIN;
            creer_mur(tab_mur,x_all,y_all,x_all+x_fin_all,y_all+y_fin_all);
        }
            for(i=0;i<NB_WALL_INIT/2;i++)
        {
            x_fin_all = rand() % DIST_WALL_INIT;
            y_fin_all = rand() % DIST_WALL_INIT;
            x_all = rand() % LARGEUR_TERRAIN;
            y_all = rand() % HAUTEUR_TERRAIN;
            creer_mur(tab_mur,x_all,y_all,x_all-x_fin_all,y_all+y_fin_all);
        }
    }
 */

	// Robot manuel
	float vitesse_man = 0;

////////////////////////////////////////////////////// Fin commande flo /////////////////////////////////////////////////////////////////////

/***************************************	Test	****************************************/






/***************************************	Fin Test	****************************************/



while(continuer)
	{
	// Gestion evenement
		//SDL_PollEvent(&event); /* On utilise PollEvent et non WaitEvent pour ne pas bloquer le programme */
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
			case SDL_KEYDOWN:
				switch(event.key.keysym.sym)
				{

					case SDLK_o:
						simu_move_robot(tab_robot,9,vitesse_man,tab_robot[9].teta-0.1);
					break;
					case SDLK_p:
						simu_move_robot(tab_robot,9,vitesse_man,tab_robot[9].teta+0.1);
					break;
					case SDLK_b:
						teta_scan = teta_scan + 10.0/180.0*M_PI;
						printf("teta_scan = %f\n",teta_scan);
					break;
					case SDLK_v:
						teta_scan = teta_scan - 10.0/180.0*M_PI;
						printf("teta_scan = %f\n",teta_scan);
					break;
					case SDLK_r:
						tab_robot[9].scan_zone = simu_scan_zone(tab_robot, tab_mur, tab_missile,9, teta_scan);
						tab_robot[9].teta_scan = teta_scan;
						tab_robot[9].count_scan = 10;
						printf("dist_scannée = %f ,type %d\n", tab_robot[9].scan_zone.dist_scan[0],tab_robot[9].scan_zone.type_scan[0]);
					break;
					case SDLK_m:
						tab_robot[9].scan_all = simu_scan_allaround(tab_robot, tab_mur,tab_missile,9);
						tab_robot[9].count_scanLarge = 10;
for (i=0; i<8; i++)
{
	printf("zone %d = %f\n",i+1,tab_robot[9].scan_all.zone_distance[i]);
}
					break;
                   			case SDLK_n:
						stat = simu_get_status(tab_robot,tab_mur,tab_missile,9);
						printf("status_result=%d\n",stat);
					break;

					case SDLK_UP:
						simu_move_robot(tab_robot,9,vitesse_man,M_PI/2);
					break;
					case SDLK_DOWN:
						simu_move_robot(tab_robot,9,vitesse_man,-M_PI/2);
					break;
					case SDLK_LEFT:
						simu_move_robot(tab_robot,9,vitesse_man,M_PI);
					break;
					case SDLK_RIGHT:
						simu_move_robot(tab_robot,9,vitesse_man,0);
					break;
					case SDLK_KP_6:
						simu_missile_shoot(tab_robot, 9, tab_missile,0);
					break;
					case SDLK_d:
						simu_missile_shoot(tab_robot, 9, tab_missile,0);
					break;

					case SDLK_KP_8:
						simu_missile_shoot(tab_robot, 9, tab_missile,M_PI/2);
					break;
					case SDLK_z:
						simu_missile_shoot(tab_robot, 9, tab_missile,M_PI/2);
					break;

					case SDLK_KP_4:
						simu_missile_shoot(tab_robot, 9, tab_missile,M_PI);
					break;
					case SDLK_q:
						simu_missile_shoot(tab_robot, 9, tab_missile,M_PI);
					break;

					case SDLK_KP_2:
						simu_missile_shoot(tab_robot, 9, tab_missile,-M_PI/2);
					break;
					case SDLK_x:
						simu_missile_shoot(tab_robot, 9, tab_missile,-M_PI/2);
					break;

					case SDLK_KP_9:
						simu_missile_shoot(tab_robot, 9, tab_missile,M_PI/4);
					break;
					case SDLK_e:
						simu_missile_shoot(tab_robot, 9, tab_missile,M_PI/4);
					break;

					case SDLK_KP_7:
						simu_missile_shoot(tab_robot, 9, tab_missile,3*M_PI/4);
					break;
					case SDLK_a:
						simu_missile_shoot(tab_robot, 9, tab_missile,3*M_PI/4);
					break;

					case SDLK_KP_3:
						simu_missile_shoot(tab_robot, 9, tab_missile,-M_PI/4);
					break;
					case SDLK_c:
						simu_missile_shoot(tab_robot, 9, tab_missile,-M_PI/4);
					break;

					case SDLK_KP_1:
						simu_missile_shoot(tab_robot, 9, tab_missile,-3*M_PI/4);
					break;
					case SDLK_w:
						simu_missile_shoot(tab_robot, 9, tab_missile,-3*M_PI/4);
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

        //gestion du temps
		tempsActuel = SDL_GetTicks();
		//printf("Temps : %d\n",tempsActuel - tempsPrecedent);
        if (tempsActuel - tempsPrecedent > 10) /* Si 30 ms se sont écoulées depuis le dernier tour de boucle */
        {
			down(sem_ID, 0);// on bloque l'acces a la memoire partagee avec le semaphore
			//	On copie les flags d'ecriture et de lecture
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
							tab_robot[i].scan_all = simu_scan_allaround(tab_robot,tab_mur,tab_missile,i);
							tab_robot[i].count_scanLarge = 10;
							sprintf(((structure_partagee*)ptr_mem_partagee)->tab_cmd[i],"%f;%f;%f;%f;%f;%f;%f;%f;",tab_robot[i].scan_all.zone_distance[0],tab_robot[i].scan_all.zone_distance[1],tab_robot[i].scan_all.zone_distance[2],tab_robot[i].scan_all.zone_distance[3],tab_robot[i].scan_all.zone_distance[4],tab_robot[i].scan_all.zone_distance[5],tab_robot[i].scan_all.zone_distance[6],tab_robot[i].scan_all.zone_distance[7]);
							break;
						case 'F' :
							sscanf(((structure_partagee*)ptr_mem_partagee)->tab_cmd[i],"F;%f;",&teta_scan);
							tab_robot[i].scan_zone = simu_scan_zone(tab_robot, tab_mur,tab_missile,i,teta_scan);
							tab_robot[i].teta_scan = teta_scan;
							tab_robot[i].count_scan = 10;
							sprintf(((structure_partagee*)ptr_mem_partagee)->tab_cmd[i],"%f;%d;",tab_robot[i].scan_zone.dist_scan[0],tab_robot[i].scan_zone.type_scan[0]);
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
			// On recopie les flags après traitement
			for(i = 0 ; i < 10 ; i++)
			{
				if(copy_flag_r[i] == 1)
				{
					((structure_partagee*)ptr_mem_partagee)->flag_r[i] = copy_flag_r[i];
					((structure_partagee*)ptr_mem_partagee)->flag_w[i] = copy_flag_w[i];
				}
			}
			up(sem_ID, 0);// On libere le semaphore

			simu_robot_update(tab_robot, tab_mur);
			simu_missile_update(tab_missile, tab_robot,tab_mur,tab_impact,&Context);
			refresh_screen(tab_robot,tab_missile,tab_mur,tab_impact,&Context);
			tempsPrecedent = tempsActuel;
        }
	}
	// close serveur
	down(sem_ID, 0);// on bloque l'acces a la memoire partagee avec le semaphore
	((structure_partagee*)ptr_mem_partagee)->game_status = 1;
	up(sem_ID, 0);// On libere le semaphore

	//	Une fois sortie de la boucle, je détache mon segment mémoire de mon processus, et quand tous les processus en auront fait autant, ce segment mémoire sera détruit.
	shmdt(ptr_mem_partagee);
	sem_delete(sem_ID);

	close_graphique(&Context);
	sleep(1);

	return 1;
}
/***************************************	END MAIN	****************************************/

void init_robot(s_robot *tab_robot)
{
    srand(time(NULL));
	//initialisation des robots
	int i = 0;
	for(i=0;i<NB_ROBOT_MAX;i++)
	{
		tab_robot[i].exist = 0;
		tab_robot[i].count_scan = 0;
		tab_robot[i].count_scanLarge = 0;
		tab_robot[i].life_point = 100;
		tab_robot[i].x = rand() % LARGEUR_TERRAIN;
        tab_robot[i].y = rand() % HAUTEUR_TERRAIN;
		tab_robot[i].teta = M_PI/4;
	}
}
