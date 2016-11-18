# MAKEFILE robot craetion le 18/11/16


# options de compilation
CC = gcc
CCFLAGS = -Wall 

# LIBSDIR = -L../lib 
# LIBS = -lm -lSDL2-2.0 -lSDL2_ttf-2.0 -lSDL2_image-2.0 -lSDL2_gfx
# INCDIRS = -I../include


# fichiers du projet
SRC_ROBOT = robot.c
SRC_CLIENT = ../API_Client/
SRC_CONFIG = ../Serveur/config.h


OBJ_ROBOT = $(SRC_ROBOT:.c=.o)
OBJ_CLIENT = $(SRC_CLIENT:.c=.o)

EXEC =robot.out


# règle initiale
 all: $(EXEC)


# dépendance des .h
client.o : $(SRC_CLIENT)client.c  $(SRC_CLIENT)client.h $(SRC_CONFIG)
robot.o : robot.c $(SRC_CLIENT)client.h $(SRC_CONFIG)


# règles de compilation
%.o: %.c
	$(CC) $(CCFLAGS) -o $@ -c $< 
	
# règles d'édition de liens

 $(EXEC):  $(OBJ_ROBOT)
	$(CC) -o $@ $^ 

# règles supplémentaires
 clean:
	rm -rf $(OBJ_ROBOT) $(OBJ_CLIENT)client.c
#mproper:
	rm -rf $(EXEC) $(OBJ_ROBOT) $(OBJ_CLIENT) client.c


