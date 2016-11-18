

# options de compilation
CC = gcc
CCFLAGS = -Wall # $(INCDIRS) $(LIBSDIR)

 LIBSDIR = -L../lib 
# LIBS = -lm -lSDL2-2.0 -lSDL2_ttf-2.0 -lSDL2_image-2.0 -lSDL2_gfx
# INCDIRS = -I../include


# fichiers du projet
#SRC_ROBOT = robot.c
SRC_CLIENT = client.c
SRC_CONFIG = ../Serveur/config.h


#OBJ_ROBOT = $(SRC_ROBOT:.c=.o)
OBJ_CLIENT = $(SRC_CLIENT:.c=.o)

# EXEC =robot.out


# règle initiale
# all: $(EXEC)


# dépendance des .h
client.o : client.c client.h ../Serveur/config.h
# robot.o : robot.c client.c client.h config.h


# règles de compilation
%.o: %.c
	$(CC) $(CCFLAGS) -o $@ -c $< 
	
# règles d'édition de liens

# $(EXEC):  $(OBJ_ROBOT) 
	#$(CC) -o $@ $^ 

# règles supplémentaires
# clean:
	#rm -rf $(OBJ_ROBOT) $(OBJ_CLIENT)
#mproper:
	#rm -rf $(EXEC) $(OBJ_ROBOT) $(OBJ_CLIENT)


