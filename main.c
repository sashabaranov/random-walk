#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>
#include <time.h>


int rnd(int min, int max, unsigned short *state)
{
  return nrand48(state) % max + min;
}

int step(int offset, int dim, int grid_size, unsigned short *state, int N)
{
    int k, d;
    for(k = 0; k < dim; ++k) 
    {
        d = pow(N, k) * rnd(-1, 3, state);
        if(offset + d > 0 && offset + d <= grid_size) offset += d;    
    }
    return offset;
}

void regenerate(int* grid, int grid_size, int cops)
{
    memset(grid, 0, grid_size * sizeof(int));

    int i,j;
    for(i = 0; i < cops; ++i) grid[rand() % grid_size] = 1;

    /*printf("Regenerated! \n data = {");
    for(i = 0; i < 10; i++) 
    {
        for(j = 0; j < 10; j++) 
        {
            printf("{%d, %d, %d}, ", i, j, grid[10*i + j]);
        }
    }*/

}

int main(int argc, char *argv[])
{
    double start = omp_get_wtime();
    
    srand ( time(NULL) );

    /********************
    * Initial variables *
    ********************/

    int dim, N, grid_size; // dimension count, size of grid, n^dim * sizeof(int)
    int cops; // cops count
    int K;    // cops regenerations count
    int M;    // total simulations count per grid regeneration (Totally K*M simulations)
    int i, j, k, d; // temp vars
    int offset = 0 ; // drunkard offset in array
    int *grid, *stats;
    int threads_num, tid;

    // for randomization:
    unsigned short state[3];
    unsigned int seed;

    // init var(add getopt later)
    dim = 2;
    N = 10;
    cops = 2;
    K = 10000;
    M = 500;
    

    // Memory alloc
    grid_size = pow(N, dim);

    grid = malloc(grid_size * sizeof(int));
    memset(grid, 0, grid_size);

    stats = calloc(K, sizeof(int));

    /********************
    *     Main loop     *
    ********************/

    for(i = 0; i < K; ++i)
    {
        regenerate(grid, grid_size, cops);

        #pragma omp parallel shared(grid, stats) private(j, k, d, tid, threads_num, state, seed) firstprivate(i, grid_size, K, M, N, dim, offset)
        {
            tid  = omp_get_thread_num();
            threads_num = omp_get_num_threads();

            // seed magic
            seed = time(NULL) + (unsigned int) omp_get_thread_num();
            memcpy(state, &seed, sizeof(seed));

            int* drunkards = calloc(M/threads_num, sizeof(int)); //drunkards offsets
            int go_flag = M/threads_num; // keep simulating
            int count; // counting not dead

            while(go_flag)
            {
                count = 0;

                for(j = 0; j < M/threads_num; ++j)
                {
                    if(drunkards[j] != -1)
                    {
                        ++ count;
                        drunkards[j] = step(drunkards[j], dim, grid_size, state, N);

                        //if(tid == 0 && j == 0) printf("> %d\n", drunkards[j]);
                        
                        if(grid[drunkards[j]] == 1)
                        {
                            //if(tid == 0 && j == 0) printf("DIE %d\n", drunkards[j]);
                            drunkards[j] = -1; // die
                        } 

                        if(drunkards[j] == grid_size) 
                        {
                            //printf("%d ACHIEVE %d\n", tid,  drunkards[j]);
                            stats[i] += 1;
                            drunkards[j] = -1;
                        }
                    }
                }
                if(count == 0) go_flag = 0;
            }
            free(drunkards);
        }
    }

    double avg, stddev;

    //for(i = 0; i < K; ++i) printf("%f\n", (float)stats[i]/M);
    
    for(i = 0; i < K; ++i) avg += (double) stats[i] / M;
    
    avg /= K;

    for(i = 0; i < K; ++i) stddev += ((double)stats[i]/M - avg) * ((double)stats[i]/M - avg);
    
    stddev = sqrt(stddev / K);


    printf("STATS: %f +/- %f\n", avg, stddev);


    double end = omp_get_wtime();
    printf("TIME : %s %f\n", getenv("OMP_NUM_THREADS"), end - start);

    free(grid);
    return 0;
}