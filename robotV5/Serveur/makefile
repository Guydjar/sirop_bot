#MAKEFILE pour Serveur et Client 03/11/16 V1.4



# options de compilation
CC = gcc
CCFLAGS = -Wall $(INCDIRS) 
LIBSDIR = -Llib
LIBS = -lm -lSDL2-2.0 -lSDL2_ttf-2.0 -lSDL2_image-2.0 -lSDL2_gfx
INCDIRS = -I../include


# fichiers du projet
SRC_SIMU = main.c simu.c wall.c graphique.c semaphore.c
SRC_SERVEUR = semaphore.c serveur.c

OBJ_SIMU = $(SRC_SIMU:.c=.o)
OBJ_SERVEUR = $(SRC_SERVEUR:.c=.o)

EXEC_SIMU = simu.out
EXEC_SERVEUR = serveur.out

# règle initiale
all: $(EXEC_SIMU) $(EXEC_SERVEUR)

# dépendance des .h

wall.o : wall.h wall.c config.h
graphique.o : graphique.c graphique.h config.h robot_simu.h
semaphore.o : semaphore.c semaphore.h config.h robot_simu.h
simu.o : simu.c simu.h wall.h graphique.h config.h robot_simu.h
serveur.o : serveur.c serveur.h semaphore.h config.h robot_simu.h
client.o : client.c client.h config.h
main.o: main.c semaphore.h config.h robot_simu.h wall.h simu.h graphique.h

# règles de compilation
%.o: %.c
	$(CC) $(CCFLAGS) -o $@ -c $<
	
# règles d'édition de liens
$(EXEC_SIMU):  $(OBJ_SIMU)
	$(CC) -o $@ $^ $(LIBS) $(LIBSDIR) 
$(EXEC_SERVEUR):  $(OBJ_SERVEUR)
	$(CC) -o $@ $^ $(LIBS) $(LIBSDIR) 

# règles supplémentaires
clean:
	rm -rf $(OBJ_SIMU) $(OBJ_SERVEUR) serveur.log
mproper:
	rm -rf $(EXEC_SIMU) $(EXEC_SERVEUR) $(OBJ_SIMU) $(OBJ_SERVEUR) serveur.log


