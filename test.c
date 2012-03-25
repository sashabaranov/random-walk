#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main()
{
    srand ( time(NULL) );
    int dim = 3;
    int N = 3;
    int cops = 4;
    int K = 2000;
    int M = 100000;
    int grid_size;
    int *grid;

    grid_size = pow(N, dim);

    grid = malloc(grid_size * sizeof(int));
    memset(grid, 0, grid_size * sizeof(int));


    int pos, count = 0;
    int i,j;
    while(count < cops)
    {
         pos = rand() % grid_size;

         if(grid[pos] != 1)
         {
            grid[pos] = 1;
            ++ count;
         }
    }

    printf("Regenerated! \n ListPlot3D[{");
    for(i = 0; i < 10; i++) 
    {
        for(j = 0; j < 10; j++) 
        {
            printf("{%d, %d, %d}, ", i, j, grid[10*i + j]);
        }
    }

}