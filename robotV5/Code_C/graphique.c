// Version 3.2 du 11/01/16

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include "graphique.h"
#include "config.h"


// init graphique, texture, font and ttf
int init_graphique(context_video* context)
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0 )
    {
        fprintf(stdout,"SDL init error : %s\n",SDL_GetError());
        return -1;
    }

    // Initialize SDL_ttf
    if( TTF_Init() != 0 )
    {
        printf( "SDL_ttf init Error : %s\n", TTF_GetError() );
        return -1;
    }

    // Create Window application
    //context->pWindow = SDL_CreateWindow("Projet Sirop Bot",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,LARGEUR_ECRAN,HAUTEUR_ECRAN,SDL_WINDOW_SHOWN));
    if(( context->pWindow = SDL_CreateWindow("Projet Sirop Bot",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,LARGEUR_ECRAN,HAUTEUR_ECRAN,SDL_WINDOW_SHOWN) )!=NULL)
    {

        SDL_SetWindowIcon(context->pWindow,SDL_LoadBMP("../graphique/images/tank.bmp"));

        // Create Renderer   if Virtual machine select SDL_RENDERER_SOFTWARE flag else select SDL_RENDERER_ACCELERATED flag
        if ( (context->pRenderer = SDL_CreateRenderer(context->pWindow,-1,SDL_RENDERER_ACCELERATED))!=NULL )
        {
            // Load field texture
            if ( (context->pWarZone = IMG_LoadTexture(context->pRenderer,"../graphique/images/sol.bmp"))!=0)
            {
                SDL_Rect dest = {MARGE_ECRAN,MARGE_ECRAN, LARGEUR_TERRAIN, HAUTEUR_TERRAIN};
                SDL_RenderCopy(context->pRenderer,context->pWarZone,NULL,&dest); // Copie du sprite grâce au SDL_Renderer
            }
            else
            {
                fprintf(stdout,"WarZone load error : (%s)\n",SDL_GetError());
                return -1;
            }

            // Load missile texture
            SDL_Surface* pSprite1 = SDL_LoadBMP("../graphique/images/Missile.bmp");
            SDL_SetColorKey(pSprite1, SDL_TRUE, SDL_MapRGB(pSprite1->format, 255, 255, 255)); // Set white color as transparent
            if ( pSprite1 )
            {
                context->pMissile = SDL_CreateTextureFromSurface(context->pRenderer,pSprite1); // Create texture
                if ( !&context->pMissile )
                {
                    fprintf(stdout,"Missile texture load error (%s)\n",SDL_GetError());
                    return -1;
                }
                SDL_FreeSurface(pSprite1); // free pSprite
            }
            else
            {
                fprintf(stdout,"pSprite Missile load error : (%s)\n",SDL_GetError());
                return -1;
            }

            // Load scanfin
            SDL_Surface* pSprite2 = SDL_LoadBMP("../graphique/images/ScanFin.bmp");
            SDL_SetColorKey(pSprite2, SDL_TRUE, SDL_MapRGB(pSprite2->format, 255, 255, 255)); // Set white color as transparent
            if ( pSprite2 )
            {
                context->pScanFin = SDL_CreateTextureFromSurface(context->pRenderer,pSprite2); // Create texture
                if ( !&context->pScanFin )
                {
                    fprintf(stdout,"ScanFin texture load error (%s)\n",SDL_GetError());
                    return -1;
                }
                SDL_FreeSurface(pSprite2); // free pSprite
            }
            else
            {
                fprintf(stdout,"pSprite ScanFin load error : (%s)\n",SDL_GetError());
                return -1;
            }
            SDL_SetTextureAlphaMod(context->pScanFin,127);

            // Load tourelle texture
            SDL_Surface* pSprite3 = SDL_LoadBMP("../graphique/images/Tourelle.bmp");
            SDL_SetColorKey(pSprite3, SDL_TRUE, SDL_MapRGB(pSprite3->format, 255, 255, 255)); // Set white color as transparent
            if ( pSprite3 )
            {
                context->pTourelle = SDL_CreateTextureFromSurface(context->pRenderer,pSprite3); // Create texture
                if ( !&context->pTourelle )
                {
                    fprintf(stdout,"Tourelle texture load error (%s)\n",SDL_GetError());
                    return -1;
                }
                SDL_FreeSurface(pSprite3); // free pSprite
            }
            else
            {
                fprintf(stdout,"pSprite Tourelle load error : (%s)\n",SDL_GetError());
                return -1;
            }

            // Load ScanLarge texture
            SDL_Surface* pSprite4 = SDL_LoadBMP("../graphique/images/ScanLarge.bmp");
            SDL_SetColorKey(pSprite4, SDL_TRUE, SDL_MapRGB(pSprite4->format, 255, 255, 255)); // Set white color as transparent

            if ( pSprite4 )
            {
                context->pScanLarge = SDL_CreateTextureFromSurface(context->pRenderer,pSprite3); // Create texture
                if ( !&context->pTourelle )
                {
                    fprintf(stdout,"ScanLarge texture load error (%s)\n",SDL_GetError());
                    return -1;
                }
                SDL_FreeSurface(pSprite4); // free pSprite
            }
            else
            {
                fprintf(stdout,"pSprite ScanLarge load error : (%s)\n",SDL_GetError());
                return -1;
            }
            SDL_SetTextureAlphaMod(context->pScanLarge,127);

            // Load robot texture
            int robot_color;
            int i;
            char robot_filename[40];
            for(i=0;i<NB_ROBOT_MAX;i++)
            {
                robot_color = i;
                if (i>=10)
                    robot_color = i%10;
                sprintf(robot_filename,"../graphique/images/Robot%d.bmp",robot_color);
                SDL_Surface* pSprite = SDL_LoadBMP(robot_filename);
                SDL_SetColorKey(pSprite, SDL_TRUE, SDL_MapRGB(pSprite->format, 255, 255, 255)); // Set white color as transparent

                if ( pSprite )
                {
                    context->pRobot[i] = SDL_CreateTextureFromSurface(context->pRenderer,pSprite); // Create texture
                    if ( !&context->pRobot[i] )
                    {
                        fprintf(stdout,"Robot terxture load error : (%s)\n",SDL_GetError());
                        return -1;
                    }
                    SDL_FreeSurface(pSprite); // free sprite
                }
                else
                {
                    fprintf(stdout,"sprite load error : (%s)\n",SDL_GetError());
                    return -1;
                }
            }

            // Load nb robot texture
            char nbrobot_filename[40];
            for(i=0;i<NB_ROBOT_MAX;i++)
            {
                sprintf(nbrobot_filename,"../graphique/images/%d.bmp",i+1);
                SDL_Surface* pSprite5 = SDL_LoadBMP(nbrobot_filename);
                SDL_SetColorKey(pSprite5, SDL_TRUE, SDL_MapRGB(pSprite5->format, 255, 255, 255)); // Set white color as transparent

                if (pSprite5)
                {
                    context->pNbRobot[i] = SDL_CreateTextureFromSurface(context->pRenderer,pSprite5); // Create texture
                    if ( !&context->pNbRobot[i] )
                    {
                        fprintf(stdout,"Robot terxture load error : (%s)\n",SDL_GetError());
                        return -1;
                    }
                    SDL_FreeSurface(pSprite5); // free sprite
                }
                else
                {
                    fprintf(stdout,"sprite load error : (%s)\n",SDL_GetError());
                    return -1;
                }
            }

            // Load explosion texture
            SDL_Surface* pSprite6 = SDL_LoadBMP("../graphique/images/Explosion.bmp");
            SDL_SetColorKey(pSprite6, SDL_TRUE, SDL_MapRGB(pSprite6->format, 255, 255, 255)); // Set white color as transparent
            if ( pSprite6 )
            {
                context->pExplosion = SDL_CreateTextureFromSurface(context->pRenderer,pSprite6); // Create texture
                if ( !&context->pExplosion )
                {
                    fprintf(stdout,"Explosion texture load error (%s)\n",SDL_GetError());
                    return -1;
                }
                SDL_FreeSurface(pSprite6); // free pSprite
            }
            else
            {
                fprintf(stdout,"pSprite Explosion load error : (%s)\n",SDL_GetError());
                return -1;
            }

            // Load ttf font in context
            context->font[0] = TTF_OpenFont("../graphique/font/font.ttf", 16); //this opens a font style and sets a size
            if ( !&context->font[0] )
            {
                fprintf(stdout,"Font load error : (%s)\n",TTF_GetError());
                return -1;
            }

            context->font[1] = TTF_OpenFont("../graphique/font/Just Like This Demo.ttf", 40); //this opens a font style and sets a size
            if ( !&context->font[1] )
            {
                fprintf(stdout,"Font load error : (%s)\n",TTF_GetError());
                return -1;
            }
            context->font[2] = TTF_OpenFont("../graphique/font/angrybirds-regular.ttf", 50);
            if ( !&context->font[2] )
            {
                fprintf(stdout,"Font load error : (%s)\n",TTF_GetError());
                return -1;
            }

            context->font[3] = TTF_OpenFont("../graphique/font/Bubblegum.ttf", 20);
            if ( !&context->font[3] )
            {
                fprintf(stdout,"Font load error : (%s)\n",TTF_GetError());
                return -1;
            }

            context->font[4] = TTF_OpenFont("../graphique/font/Game Robot.ttf", 20);
            if ( !&context->font[4] )
            {
                fprintf(stdout,"Font load error : (%s)\n",TTF_GetError());
                return -1;
            }

            // Load message text
            SDL_Color Black = {0, 0, 0,255};  // this is the color in rgb format, maxing out all would give you the color white, and it will be your text's color
            //SDL_Color Red = {0, 0, 0,255};  // this is the color in rgb format, maxing out all would give you the color white, and it will be your text's color
            //SDL_Color Blue = {0, 0, 255,78};  // this is the color in rgb format, maxing out all would give you the color white, and it will be your text's color

            SDL_Surface* surfaceMessage = TTF_RenderText_Solid(context->font[1], "START", Black); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first
            context->text[0]->message = SDL_CreateTextureFromSurface(context->pRenderer, surfaceMessage); //now you can convert it into a texture
            SDL_Rect Rectmessage3 = {LARGEUR_TERRAIN+(2*MARGE_ECRAN)+((LARGEUR_SCORE-surfaceMessage->w)/2),HAUTEUR_TERRAIN+(2*MARGE_ECRAN)+(((HAUTEUR_LOG/2)-surfaceMessage->h)/2),surfaceMessage->w,surfaceMessage->h};
            context->text[0]->Rect = Rectmessage3;
            context->text[0]->w = surfaceMessage->w;
            context->text[0]->h = surfaceMessage->h;
            SDL_FreeSurface(surfaceMessage); // Free sprite
            surfaceMessage = TTF_RenderText_Solid(context->font[1], "QUIT", Black); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first
            context->text[1]->message = SDL_CreateTextureFromSurface(context->pRenderer, surfaceMessage); //now you can convert it into a texture
            SDL_Rect Rectmessage4 = {LARGEUR_TERRAIN+(2*MARGE_ECRAN)+((LARGEUR_SCORE-surfaceMessage->w)/2),HAUTEUR_TERRAIN+(2*MARGE_ECRAN)+(HAUTEUR_LOG/2)+(((HAUTEUR_LOG/2)-surfaceMessage->h)/2),surfaceMessage->w,surfaceMessage->h};
            context->text[1]->Rect = Rectmessage4;
            context->text[1]->w = surfaceMessage->w;
            context->text[1]->h = surfaceMessage->h;

            SDL_FreeSurface(surfaceMessage); // Free sprite
        }
        else
        {
            fprintf(stdout,"redender load error : (%s)\n",SDL_GetError());
            return -1;
        }
    }
    else
    {
        fprintf(stderr,"Window load error : %s\n",SDL_GetError());
        return -1;
    }
    return 0;
}

// Draw score zone : name player and life points
void draw_score_update(context_video* context,s_robot *tab_robot)
{
    int i;
    char buffer[400];
    char buffer2[40];

    SDL_Color Black = {0, 0, 0,255};  // this is the color in rgb format, maxing out all would give you the color white, and it will be your text's color

    SDL_SetRenderDrawColor(context->pRenderer, 245, 245 ,165, 255);

    SDL_Rect score = {LARGEUR_TERRAIN+(2*MARGE_ECRAN),MARGE_ECRAN,LARGEUR_SCORE,HAUTEUR_TERRAIN};//It's a rectangle  , size 250*250 and position (0;0)
    SDL_RenderFillRect(context->pRenderer,&score);//To draw one rectangle

    // Create liste of name
    sprintf(buffer,"Name player\n");
    for(i=0;i<NB_ROBOT_MAX;i++)
    {
        if(tab_robot[i].exist == 1)
        {
            sprintf(buffer2,"%s\n",tab_robot[i].name);
            strcat(buffer,buffer2);
        }
    }
    SDL_Surface *surfaceMessage = TTF_RenderText_Blended_Wrapped(context->font[BUBBLE], buffer, Black,195);
    SDL_Texture *Message = SDL_CreateTextureFromSurface(context->pRenderer, surfaceMessage);

    SDL_Rect title_name = {LARGEUR_TERRAIN+(2*MARGE_ECRAN)+5,MARGE_ECRAN+5,surfaceMessage->w,surfaceMessage->h}; //create a rect
    SDL_RenderCopy(context->pRenderer,Message, NULL, &title_name); //you put the renderer's name first
    SDL_DestroyTexture(Message); // Free Texture
    SDL_FreeSurface(surfaceMessage); // Free sprite

    // Create liste of life pts
    sprintf(buffer,"Life pts\n");
    for(i=0;i<NB_ROBOT_MAX;i++)
    {
        if(tab_robot[i].exist == 1)
        {
            sprintf(buffer2,"%d\n",tab_robot[i].life_point);
            strcat(buffer,buffer2);
        }
    }
    surfaceMessage = TTF_RenderText_Blended_Wrapped(context->font[BUBBLE], buffer, Black,100);
    Message = SDL_CreateTextureFromSurface(context->pRenderer, surfaceMessage);

    SDL_Rect title_life = {LARGEUR_TERRAIN+(2*MARGE_ECRAN)+200,MARGE_ECRAN+5,surfaceMessage->w,surfaceMessage->h}; //create a rect
    SDL_RenderCopy(context->pRenderer,Message, NULL, &title_life); //you put the renderer's name first

    // Free surface and texture
    SDL_DestroyTexture(Message); // Free texture
    SDL_FreeSurface(surfaceMessage); // Free sprite
}

// Draw log zone
void draw_log_update(context_video* context)
{
    int i;
    char buffer[2000];
    char buffer2[200];
    SDL_Color Black = {0, 0, 0,255};  // this is the color in rgb format, maxing out all would give you the color white, and it will be your text's color
    SDL_SetRenderDrawColor(context->pRenderer, 245, 245 ,165, 255);

    SDL_Rect log = {MARGE_ECRAN,HAUTEUR_TERRAIN+(2*MARGE_ECRAN),LARGEUR_TERRAIN,HAUTEUR_LOG};//It's a rectangle  , size 250*250 and position (0;0)
    SDL_RenderFillRect(context->pRenderer,&log);//To draw one rectangle

    // Create liste of log
    sprintf(buffer,"Log :\n");
    for(i=2;i<10;i++)
    {
        sprintf(buffer2,"%s\n",context->tab_log[i]);
        strcat(buffer,buffer2);
    }

    SDL_Surface *surfaceMessage = TTF_RenderText_Blended_Wrapped(context->font[BUBBLE], buffer, Black,LARGEUR_TERRAIN-5);
    SDL_Texture *Message = SDL_CreateTextureFromSurface(context->pRenderer, surfaceMessage);

    SDL_Rect title_name = {MARGE_ECRAN+5,HAUTEUR_TERRAIN+(2*MARGE_ECRAN)+5,surfaceMessage->w,surfaceMessage->h}; //create a rect
    SDL_RenderCopy(context->pRenderer,Message, NULL, &title_name); //you put the renderer's name first

    SDL_DestroyTexture(Message); // Free texture
    SDL_FreeSurface(surfaceMessage); // Free sprite
}

// Draw button start and quit on renderer
void draw_button(context_video* context)
{

    // Bouton START
    SDL_SetRenderDrawColor(context->pRenderer, 197, 224 ,180, 255);
    SDL_Rect start = {LARGEUR_TERRAIN+(2*MARGE_ECRAN),HAUTEUR_TERRAIN+(2*MARGE_ECRAN),LARGEUR_SCORE,HAUTEUR_LOG/2};//It's a rectangle  , size 250*250 and position (0;0)
    SDL_RenderFillRect(context->pRenderer,&start);//To draw one rectangle
    SDL_RenderCopy(context->pRenderer, context->text[0]->message, NULL, &context->text[0]->Rect); //you put the renderer's name first

    // bouton QUIT
    SDL_SetRenderDrawColor(context->pRenderer, 255, 157 ,157, 255);
    SDL_Rect quit = {LARGEUR_TERRAIN+(2*MARGE_ECRAN),HAUTEUR_TERRAIN+(2*MARGE_ECRAN)+(HAUTEUR_LOG/2),LARGEUR_SCORE,HAUTEUR_LOG/2};//It's a rectangle  , size 250*250 and position (0;0)
    SDL_RenderFillRect(context->pRenderer,&quit);//To draw one rectangle
    SDL_RenderCopy(context->pRenderer, context->text[1]->message, NULL, &context->text[1]->Rect); //you put the renderer's name first
}

// Draw War Zone on the renderer
void draw_war_zone(context_video* context)
{
    SDL_Rect dest = {MARGE_ECRAN,MARGE_ECRAN, LARGEUR_TERRAIN, HAUTEUR_TERRAIN};
    SDL_RenderCopy(context->pRenderer,context->pWarZone,NULL,&dest); // Copie sprite with SDL_Renderer
}

//Draw Robot and tourelle
void draw_robot_and_tourelle(context_video* context,s_robot *tab_robot)
{
    int i, w, h;
    float angle;
    SDL_Point center;
    int xcercle;
    int ycercle;

    for(i=0;i<NB_ROBOT_MAX;i++)
    {
        if (tab_robot[i].exist == 1)
        {
            if(tab_robot[i].count_scanLarge>0)
            {
                tab_robot[i].count_scanLarge--;
                SDL_QueryTexture(context->pScanLarge, NULL, NULL, &w, &h);
                SDL_Rect dest_scanLarge = {tab_robot[i].x + MARGE_ECRAN - 22,HAUTEUR_TERRAIN + MARGE_ECRAN - tab_robot[i].y -22, w,h};
                SDL_RenderCopy(context->pRenderer,context->pScanLarge,NULL,&dest_scanLarge); // Copie sprite with SDL_Renderer
            }
            if(tab_robot[i].count_scan>0)
            {
                tab_robot[i].count_scan--;
                SDL_QueryTexture(context->pScanFin, NULL, NULL, &w,&h);
                angle = 90 - (180 * tab_robot[i].teta_scan / M_PI);
                center.x = w/2;
                center.y = h/2;
                SDL_Rect dest_scan = {tab_robot[i].x + MARGE_ECRAN - 50,HAUTEUR_TERRAIN + MARGE_ECRAN - tab_robot[i].y -50, w,h};
                SDL_RenderCopyEx(context->pRenderer,context->pScanFin,NULL,&dest_scan,angle,&center,SDL_FLIP_NONE); // Copie sprite with SDL_Renderer
                //xcercle = MARGE_ECRAN+tab_robot[i].scan_zone.x;
                //ycercle = HAUTEUR_TERRAIN + MARGE_ECRAN - tab_robot[i].scan_zone.y;
                //filledCircleRGBA(context->pRenderer,xcercle,ycercle,10,141,255,141,255);
            }

            // Load player number
            SDL_QueryTexture((SDL_Texture*)context->pNbRobot[i], NULL, NULL, &w, &h);
            SDL_Rect dest_nb = {tab_robot[i].x + MARGE_ECRAN - 23,HAUTEUR_TERRAIN + MARGE_ECRAN - tab_robot[i].y - 23, 45,45};
            SDL_RenderCopy(context->pRenderer,(SDL_Texture*)context->pNbRobot[i],NULL,&dest_nb); //Copie sprite with SDL_Renderer

            // Load robot
            SDL_QueryTexture((SDL_Texture*)context->pRobot[i], NULL, NULL, &w, &h);
            angle = 90 - (180 * tab_robot[i].teta / M_PI);
            center.x = w/2;
            center.y = h/2;
            SDL_Rect dest = {tab_robot[i].x + MARGE_ECRAN - 11,HAUTEUR_TERRAIN + MARGE_ECRAN - tab_robot[i].y - 11, w,h};
            SDL_RenderCopyEx(context->pRenderer,(SDL_Texture*)context->pRobot[i],NULL,&dest,angle,&center,SDL_FLIP_NONE); //Copie sprite with SDL_Renderer

            SDL_QueryTexture(context->pTourelle, NULL, NULL, &w, &h);
            angle = 90 - (180 * tab_robot[i].teta_tir / M_PI);
            center.x = w/2;
            center.y = h/2;
            SDL_Rect dest_tour = {tab_robot[i].x + MARGE_ECRAN - 15,HAUTEUR_TERRAIN + MARGE_ECRAN - tab_robot[i].y -15, w,h};
            SDL_RenderCopyEx(context->pRenderer,context->pTourelle,NULL,&dest_tour,angle,&center,SDL_FLIP_NONE); // Copie sprite with SDL_Renderer
        }
        if(tab_robot[i].count_explosion>0)
        {
            tab_robot[i].count_explosion--;
            SDL_QueryTexture((SDL_Texture*)context->pExplosion, NULL, NULL, &w, &h);
            SDL_Rect dest_ex = {tab_robot[i].x + MARGE_ECRAN - 10,HAUTEUR_TERRAIN + MARGE_ECRAN - tab_robot[i].y - 10, 20,20};
            SDL_RenderCopy(context->pRenderer,(SDL_Texture*)context->pExplosion,NULL,&dest_ex); //Copie sprite with SDL_Renderer
        }
    }
}

// Load missile
void draw_missile(context_video* context,s_missile *tab_missile,s_impact *tab_impact)
{
    int i, w, h;
    float angle;
    SDL_Point center;

    SDL_QueryTexture(context->pMissile, NULL, NULL, &w, &h);
    for(i=0;i<NB_ROBOT_MAX*NB_MISSILE_ALLOWED;i++)
    {
        if (tab_missile[i].exist == 1)
        {
            angle = 90 - (180 * tab_missile[i].teta / M_PI);
            center.x = 11;
            center.y = 11;
            SDL_Rect dest = {tab_missile[i].x - MISSILE_LENGH/2 * cos(tab_missile[i].teta) + MARGE_ECRAN - (RAYON_ROBOT+1),HAUTEUR_TERRAIN + MARGE_ECRAN - (tab_missile[i].y - MISSILE_LENGH/2*sin(tab_missile[i].teta)) - (RAYON_ROBOT+1), w,h};
            SDL_RenderCopyEx(context->pRenderer,context->pMissile,NULL,&dest,angle,&center,SDL_FLIP_NONE); // Copie sprite with SDL_Renderer
        }

    }

    SDL_QueryTexture(context->pExplosion, NULL, NULL, &w, &h);
    for(i=0;i<NB_ROBOT_MAX*NB_MISSILE_ALLOWED;i++)
    {
        if (tab_impact[i].count_impact>0)
        {
            tab_impact[i].count_impact--;
            SDL_Rect dest_impact = {tab_impact[i].x + MARGE_ECRAN - 5,HAUTEUR_TERRAIN + MARGE_ECRAN - tab_impact[i].y - 5, 10,10};
            SDL_RenderCopy(context->pRenderer,context->pExplosion,NULL,&dest_impact); // Copie sprite with SDL_Renderer
        }
    }

}

// Load wall
void draw_wall(context_video* context,s_mur *tab_mur)
{
    int i;
    for(i=0;i<NB_WALL_ALLOWED;i++)
    {
        if(tab_mur[i].exist == 1)
        {
            SDL_SetRenderDrawColor(context->pRenderer, 255, 0 ,0, 255);
            SDL_RenderDrawLine(context->pRenderer,MARGE_ECRAN+tab_mur[i].Xdebut,MARGE_ECRAN+HAUTEUR_TERRAIN-tab_mur[i].Ydebut,MARGE_ECRAN + tab_mur[i].Xfin,MARGE_ECRAN+HAUTEUR_TERRAIN-tab_mur[i].Yfin);
        }
    }
}

// Refresh game screen
void refresh_screen(s_robot *tab_robot,s_missile *tab_missile,s_mur *tab_mur,s_impact *tab_impact, context_video* context)
{
	SDL_SetRenderDrawColor(context->pRenderer, 0,0,0 , 255);
	SDL_RenderClear(context->pRenderer);
    // Load terrain
    draw_war_zone(context);

    // Load wall
    draw_wall(context,tab_mur);

    // Draw robot
    draw_robot_and_tourelle(context,tab_robot);

    // Load missile
    draw_missile(context,tab_missile,tab_impact);

    // Score log and button
    draw_score_update(context,tab_robot);
    draw_log_update(context);
    draw_button(context);

	SDL_RenderPresent(context->pRenderer); // Affichage
}

// Wait client screen
void refresh_screen_init(context_video* context,s_robot *tab_robot,int nbplayeur)
{
	SDL_SetRenderDrawColor(context->pRenderer, 0,0,0 , 255);
	SDL_RenderClear(context->pRenderer);

    // Load terrain log score and button
    draw_war_zone(context);
    draw_score_update(context,tab_robot);
    draw_log_update(context);
    draw_button(context);

	// Wait message
    SDL_Color Black = {79, 6, 224,255};  // this is the color in rgb format, maxing out all would give you the color white, and it will be your text's color

    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(context->font[WORLD], "Welcome to Sirop Bot Project", Black); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first
    SDL_Texture* Message = SDL_CreateTextureFromSurface(context->pRenderer, surfaceMessage); //now you can convert it into a texture
    SDL_Rect Message_wait_rect = {(LARGEUR_TERRAIN-surfaceMessage->w)/2,HAUTEUR_TERRAIN/5-(surfaceMessage->h/2),surfaceMessage->w,surfaceMessage->h}; //creat
    SDL_RenderCopy(context->pRenderer, Message, NULL, &Message_wait_rect); //you put the renderer's name firste a rect
    SDL_DestroyTexture(Message); // Free texture
    SDL_FreeSurface(surfaceMessage); // free sprite
    char player_connected[60];
    sprintf(player_connected,"%d Player connected",nbplayeur);

    surfaceMessage = TTF_RenderText_Solid(context->font[WORLD], player_connected, Black); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first
    Message = SDL_CreateTextureFromSurface(context->pRenderer, surfaceMessage); //now you can convert it into a texture
    SDL_Rect Message_wait2_rect = {(LARGEUR_TERRAIN-surfaceMessage->w)/2,2*(HAUTEUR_TERRAIN/5)-(surfaceMessage->h/2),surfaceMessage->w,surfaceMessage->h}; //create a rect
    SDL_RenderCopy(context->pRenderer, Message, NULL, &Message_wait2_rect); //you put the renderer's name firste a rect
    SDL_DestroyTexture(Message); // free texture
    SDL_FreeSurface(surfaceMessage); // free sprite

    surfaceMessage = TTF_RenderText_Solid(context->font[WORLD], "Press start after client connection", Black); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first
    Message = SDL_CreateTextureFromSurface(context->pRenderer, surfaceMessage); //now you can convert it into a texture
    SDL_Rect Message_wait3_rect = {(LARGEUR_TERRAIN-surfaceMessage->w)/2,3*(HAUTEUR_TERRAIN/5)-(surfaceMessage->h/2),surfaceMessage->w,surfaceMessage->h}; //create a rect
    SDL_RenderCopy(context->pRenderer, Message, NULL, &Message_wait3_rect); //you put the renderer's name firste a rect

    SDL_DestroyTexture(Message); // free texture
    SDL_FreeSurface(surfaceMessage); // freesprite
	SDL_RenderPresent(context->pRenderer); // Affichage
}

// Start screen
void start_screen(context_video* context)
{
    // Load terrain

    SDL_Texture *start= IMG_LoadTexture(context->pRenderer,"../graphique/images/init.png");
    if ( start )
    {
        SDL_Rect dest = {0,0, LARGEUR_ECRAN , HAUTEUR_ECRAN};
        SDL_RenderCopy(context->pRenderer,start,NULL,&dest); // Copie sprite with SDL_Renderer
    }
    else
    {
        fprintf(stdout,"Start load error : (%s)\n",SDL_GetError());
    }

	// Wait message
    SDL_Color White = {0, 0, 255,255};  // this is the color in rgb format, maxing out all would give you the color white, and it will be your text's color

    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(context->font[WORLD], "SIROP BOT BATTLE", White); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first
    SDL_Texture* Message = SDL_CreateTextureFromSurface(context->pRenderer, surfaceMessage); //now you can convert it into a texture
    SDL_Rect Message_wait_rect = {(LARGEUR_ECRAN-surfaceMessage->w)/2,HAUTEUR_ECRAN/8-(surfaceMessage->h/2),surfaceMessage->w,surfaceMessage->h}; //creat
    SDL_RenderCopy(context->pRenderer, Message, NULL, &Message_wait_rect); //you put the renderer's name firste a rect

    surfaceMessage = TTF_RenderText_Solid(context->font[WORLD], "Copyright UPMC E2I FC25", White); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first
    Message = SDL_CreateTextureFromSurface(context->pRenderer, surfaceMessage); //now you can convert it into a texture
    SDL_Rect Message_wait3_rect = {(LARGEUR_ECRAN-surfaceMessage->w)/2,7*(HAUTEUR_ECRAN/8)-(surfaceMessage->h/2),surfaceMessage->w,surfaceMessage->h}; //create a rect
    SDL_RenderCopy(context->pRenderer, Message, NULL, &Message_wait3_rect); //you put the renderer's name firste a rect

    SDL_DestroyTexture(start); // free texture
    SDL_DestroyTexture(Message); //free texture
    SDL_FreeSurface(surfaceMessage); //free sprite*/
	SDL_RenderPresent(context->pRenderer); // Affichage

}

// close graphique texture and font
int close_graphique(context_video* context)
{
	int i;
	for(i=0;i<NB_ROBOT_MAX;i++)
        SDL_DestroyTexture((SDL_Texture*)context->pRobot[i]);
    free(context->pRobot);
    for(i=0;i<5;i++)
        SDL_DestroyTexture((SDL_Texture*)context->text[i]->message);
    free(context->text);

    SDL_DestroyTexture(context->pTourelle); // free texture
	SDL_DestroyTexture(context->pMissile); // free texture
	SDL_DestroyTexture(context->pWarZone); // freetexture
    SDL_DestroyRenderer(context->pRenderer); // free SDL_Renderer
    SDL_DestroyWindow(context->pWindow);

    for(i=0;i<5;i++)
    {
        TTF_CloseFont(context->font[i]);
    }

    TTF_Quit();
    SDL_Quit();

    return 0;
}

void logger(context_video* context, char *str)
{
    int i;
    for(i=1;i<10;i++)
    {
        sprintf(context->tab_log[i-1],"%s",context->tab_log[i]);
    }
    sprintf(context->tab_log[9],"%s",str);
}
