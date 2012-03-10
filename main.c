#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>
#include <time.h>


inline void add_cop(int* grid, int grid_size, unsigned short *state, int gen)
{
    grid[nrand48(state) % grid_size] = gen;
}

int rnd(int min, int max, unsigned short *state)
{
  return nrand48(state) % max + min;
}


int main(int argc, char *argv[])
{
    double start = omp_get_wtime();

    /********************
    * Initial variables *
    ********************/

    int dim, N, grid_size; // dimension count, size of grid, n^dim * sizeof(int)
    int a, b;   // cops count from a to b
    int M;      // total simulations per one cops count
    int i, j, k, d; // temp vars
    int *grid, *stats;
    int threads_num, tid;
    int offset = 0 ; // drunkard offset in array

    // init var(add getopt later)
    dim = 2;
    N = 10;
    a = 4;
    b = 10;
    M = 100;


    // Memory alloc
    grid_size = pow(N, dim);

    grid = malloc(grid_size * sizeof(int));
    memset(grid, 1, grid_size);

    stats = calloc((b-a), sizeof(int));

    /********************
    *     Main loop     *
    ********************/

    #pragma omp parallel shared(grid, stats) private(i, j, k, d, tid) firstprivate(grid_size, threads_num, M, dim, a, b, offset)
    {
        tid  = omp_get_thread_num();
        threads_num = omp_get_num_threads();

        unsigned short state[3];
        unsigned int seed = time(NULL) + (unsigned int) omp_get_thread_num();
        memcpy(state, &seed, sizeof(seed));


        for(i = a; i < b; ++i)
        {
            //printf("%d: on gen %d - %d\n", tid, i, grid_size);

            #pragma omp single
            {
                // add new cops generation
                for(j = 0; j < i; ++j) add_cop(grid, grid_size, state, i);
            }

            //simulate
            for(j = 0; j < M/threads_num; ++j)
            {
                //printf("%d: Simulating %d\n", tid, i);
                while(1)
                {
                    //printf("%d: Offset %d\n", tid, offset);
                    for(k = 0; k < dim; ++k) 
                    {
                        d = pow(N, k) * rnd(-1, 3, state);
                        if(offset + d > 0 && offset + d <= grid_size) offset += d;    
                    }

                    // cop check here

                    if(offset == grid_size) 
                    {
                       
                        stats[i-a] += 1;
                        //printf("%d: Exiting: %d - %d \n", i, offset, stats[i]);
                        break;
                    }
                }
            }
        }
    }
    #pragma omp end parallel

    printf("STATS:\n");
    for(i = 0; i < (b-a); i++)
    {
        printf("%d %d/%d\n", a+i, stats[i], M/omp_get_num_threads());
    }

    double end = omp_get_wtime();
    printf("TIME : %s %f\n", getenv("OMP_NUM_THREADS"), end - start);
}