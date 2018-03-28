#define _STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include "cache.h"
#include <math.h>

CACHE cache;

CACHE *create_cache (unsigned int size, unsigned int associativity, unsigned int line_size);

//utility function to check if a number is a power of two
//used on command line input
int isPowTwo(int n)
{
   return (ceil(log2(n)) == floor(log2(n)));
}
/* Creates and initializes the cache structure based on the size (KB), associativity, and
 * line size (Bytes) parameters.
 * */
CACHE *create_cache (unsigned int size, unsigned int associativity, unsigned int line_size)
{
    if (!isPowTwo(size) || !isPowTwo(associativity) || !isPowTwo(line_size)) {
      return NULL;
    }
    size *= 1024;
    cache.size = size;
    cache.associativity = associativity;
    cache.line_size = line_size;

    cache.num_sets = (size / line_size) / associativity;

    //initialize the LRU matrices to all zeros
    for (int i=0; i< sizeof(cache.LRU[0])/sizeof(unsigned int); i++) {
          for (int j=0; j< sizeof(cache.LRU[0][0])/sizeof(unsigned int); j++) {
              for (int k=0; k< sizeof(cache.LRU[0][0])/sizeof(unsigned int); k++) {
                  cache.LRU[i][j][k] = 0;
              }
          }
      }

    return &cache;
}
