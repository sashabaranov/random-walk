#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>
#include <time.h>
#include <unistd.h>

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

    int pos, count = 0;
    while(count < cops)
    {
         pos = rand() % grid_size;

         if(grid[pos] != 1)
         {
            grid[pos] = 1;
            ++ count;
         }
    }
    //for(i = 0; i < cops; ++i) grid[rand() % grid_size] = 1;

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
    int i, j; // temp vars
    int *grid, *stats;
    int threads_num, tid;
    int go_flag, count;
    int shout;

    // for randomization:
    unsigned short state[3];
    unsigned int seed;

    // init var(add getopt later)
    /*dim = 3;
    N = 4;
    cops = 1;
    K = 500;
    M = 100000;*/
    
    int res = 0;

    char* usage = "Usage:\n"
                  "\t-d <number>\tDimensions count\n"
                  "\t-N <number>\tGrid size\n"
                  "\t-c <number>\tCops count\n"
                  "\t-K <number>\tGrid regenerations count\n"
                  "\t-M <number>\tSimulations count per regeneration\n";

    while((res = getopt(argc, argv, "d:N:c:K:M:sh")) != -1)
    {
        switch(res)
        {
            case 'd': dim   = atoi(optarg); break;
            case 'N': N     = atoi(optarg); break;
            case 'c': cops  = atoi(optarg); break;
            case 'K': K     = atoi(optarg); break;
            case 'M': M     = atoi(optarg); break;
            case 'h': printf("%s", usage); return 0;
        }
    }



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
        //if(i%100 == 0) printf("Sim #%d\n", i);
        regenerate(grid, grid_size, cops);

        #pragma omp parallel shared(grid, stats) private(j, tid, threads_num, state, seed, go_flag, count) firstprivate(i, grid_size, K, M, N, dim)
        {
            tid  = omp_get_thread_num();
            threads_num = omp_get_num_threads();

            // seed magic
            seed = time(NULL) + (unsigned int) omp_get_thread_num();
            memcpy(state, &seed, sizeof(seed));

            int* drunkards = calloc(M/threads_num, sizeof(int)); //drunkards offsets
            go_flag = M/threads_num; // keep simulating

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

    //for(i = 0; i < K; ++i) printf("%d\n", stats[i]);
    
    for(i = 0; i < K; ++i) avg += (double) stats[i] / M;
    
    avg /= K;

    for(i = 0; i < K; ++i) stddev += ((double)stats[i]/M - avg) * ((double)stats[i]/M - avg);
    
    stddev = sqrt(stddev / K);


    //printf("STATS: %f +/- %f\n", avg, stddev);


    double end = omp_get_wtime();
    printf("%d %f %f %f %d %d\n", cops, avg, stddev, end - start, M, K);

    //printf("TIME : %s %f\n", getenv("OMP_NUM_THREADS"), end - start);

    free(grid);
    return 0;
}