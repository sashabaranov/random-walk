#include <stdio.h>
#include <stdlib.h>

int rndd(int min, int max, unsigned short *state)
{
  return nrand48(state) % max + min;
}

int main()
{
    unsigned short state[3];
    unsigned int seed = time(NULL);
    memcpy(state, &seed, sizeof(seed));

    srand48(state);
    for(int i = 0; i< 500; i++) printf("%d\n", rndd(-1,3, state));
    return 0;
}