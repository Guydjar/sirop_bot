#include <stdio.h>
#include <stdlib.h>
#include <time.h> 


int main(void)
{
    aleatoire(250,250);
}


void aleatoire (xmax,ymax)
{

    int x_aleatoire, y_aleatoire;
    srand(time(NULL));
    x_aleatoire = rand() % xmax;
    y_aleatoire = rand() % ymax;
    printf("%d ",x_aleatoire);
    printf("%d ",y_aleatoire);

}
