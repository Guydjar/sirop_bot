# options de compilation
CC = gcc
CCFLAGS = -Wall  $(INCDIRS) $(LIBSDIR)

 LIBSDIR = -L../lib 
 LIBS = -lm -lSDL2-2.0 -lSDL2_ttf-2.0 -lSDL2_image-2.0 -lSDL2_gfx
 INCDIRS = -I../include


# fichiers du projet
SRC_ROBOT = robot.c ../API_Client/client.c


OBJ_ROBOT = $(SRC_ROBOT:.c=.o)


EXEC =robot.out


# règle initiale
all: $(EXEC)


# dépendance des .h

../API_Client/client.o :  ../API_Client/client.h ../Serveur/config.h
robot.o :  ../API_Client/client.h ../Serveur/config.h

# règles de compilation
%.o: %.c
	$(CC) $(CCFLAGS) -o $@ -c $< 
	
	
# règles d'édition de liens

$(EXEC):  $(OBJ_ROBOT) 
	$(CC) -o $@ $^ 

# règles supplémentaires
clean:
	rm -rf $(OBJ_ROBOT) $(OBJ_CLIENT)
mproper:
	rm -rf $(EXEC) $(OBJ_ROBOT) $(OBJ_CLIENT)


